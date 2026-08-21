// Zero-dependency docs viewer. Node stdlib only + a vendored copy of marked.
//   node tools/docs-server.js [port]
// Works identically on macOS and Windows.

const http = require('http');
const fs = require('fs');
const fsp = require('fs/promises');
const path = require('path');

const ROOT = path.resolve(__dirname, '..');
const DOCS = path.join(ROOT, 'docs');
const SHELL = path.join(__dirname, 'docs-shell.html');
const VENDOR = path.join(__dirname, 'vendor', 'marked.min.js');
const PORT = Number(process.argv[2] || process.env.PORT || 4321);

const SAFE_DOC = /^[A-Za-z0-9._-]+\.md$/;

// Fallback only — the doc's own H1 is the canonical title.
function titleFromFilename(name) {
  const m = name.match(/^(\d+)-(.*)\.md$/);
  if (!m) return name.replace(/\.md$/, '');
  const words = m[2].split('-').map(w => w.charAt(0).toUpperCase() + w.slice(1));
  return m[1] + ' — ' + words.join(' ');
}

async function titleOf(file) {
  try {
    const text = await fsp.readFile(path.join(DOCS, file), 'utf8');
    const h1 = text.match(/^#\s+(.+?)\s*$/m);
    if (h1) return h1[1];
  } catch {
    /* fall through */
  }
  return titleFromFilename(file);
}

async function listDocs() {
  const entries = await fsp.readdir(DOCS);
  const files = entries.filter(f => SAFE_DOC.test(f)).sort();
  return Promise.all(files.map(async f => ({ file: f, title: await titleOf(f) })));
}

function send(res, status, type, body) {
  res.writeHead(status, { 'Content-Type': type, 'Cache-Control': 'no-store' });
  res.end(body);
}

const server = http.createServer(async (req, res) => {
  try {
    const p = decodeURIComponent(new URL(req.url, 'http://localhost').pathname);

    if (p === '/' || p === '/index.html') {
      return send(res, 200, 'text/html; charset=utf-8', await fsp.readFile(SHELL));
    }

    if (p === '/api/docs') {
      const body = JSON.stringify(await listDocs());
      return send(res, 200, 'application/json; charset=utf-8', body);
    }

    if (p === '/vendor/marked.min.js') {
      const body = await fsp.readFile(VENDOR);
      return send(res, 200, 'application/javascript; charset=utf-8', body);
    }

    if (p.startsWith('/docs/')) {
      const name = p.slice('/docs/'.length);
      if (!SAFE_DOC.test(name)) return send(res, 400, 'text/plain', 'Bad request');
      const full = path.join(DOCS, name);
      if (path.dirname(full) !== DOCS) return send(res, 400, 'text/plain', 'Bad request');
      const body = await fsp.readFile(full, 'utf8');
      return send(res, 200, 'text/markdown; charset=utf-8', body);
    }

    send(res, 404, 'text/plain', 'Not found');
  } catch (err) {
    if (err && err.code === 'ENOENT') return send(res, 404, 'text/plain', 'Not found');
    send(res, 500, 'text/plain', 'Server error: ' + (err && err.message));
  }
});

for (const required of [DOCS, SHELL, VENDOR]) {
  if (!fs.existsSync(required)) {
    console.error('Missing required path: ' + required);
    process.exit(1);
  }
}

server.listen(PORT, '127.0.0.1', () => {
  console.log('docs  ->  http://localhost:' + PORT);
  console.log('serving ' + DOCS);
  console.log('Ctrl-C to stop');
});
