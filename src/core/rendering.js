/**
 * 내장 렌더링 모듈입니다. / Built-in rendering module.
 * Markdown/PDF를 별도 확장 없이 처리하는 기본 경로를 제공합니다.
 * Provides built-in paths for Markdown/PDF without extra extensions.
 */
export class BuiltInRenderer {
  renderMarkdown(markdownText) {
    // 최소 구현: 데모를 위한 단순 변환입니다.
    // Minimal implementation: simple conversion for demonstration.
    return markdownText
      .replace(/^###\s(.+)$/gm, '<h3>$1</h3>')
      .replace(/^##\s(.+)$/gm, '<h2>$1</h2>')
      .replace(/^#\s(.+)$/gm, '<h1>$1</h1>')
      .replace(/\*\*(.+?)\*\*/g, '<strong>$1</strong>');
  }

  renderPdf(pdfSource) {
    // 실제 제품에서는 웹뷰 네이티브 렌더러 또는 PDF.js 경로로 연결합니다.
    // In production, this connects to native webview rendering or a PDF.js path.
    return {
      engine: 'native-or-pdfjs',
      source: pdfSource,
      status: 'ready',
    };
  }
}
