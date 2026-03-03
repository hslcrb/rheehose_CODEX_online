/**
 * 확장 관리자입니다. / Extension manager.
 * Open VSX 조회와 직접 확장 파일 설치를 지원합니다.
 * Supports Open VSX lookup and direct extension-file installation.
 */
export class ExtensionManager {
  constructor({ registryUrl }) {
    this.registryUrl = registryUrl;
  }

  getOpenVsxSearchUrl(query) {
    return `${this.registryUrl}/api/-/search?query=${encodeURIComponent(query)}`;
  }

  importExtensionFile(filePath) {
    const supported = ['.vsix', '.zip'];
    const isSupported = supported.some((ext) => filePath.endsWith(ext));

    if (!isSupported) {
      throw new Error(`Unsupported extension package: ${filePath}`);
    }

    return {
      action: 'install',
      source: 'direct-file',
      filePath,
      accepted: true,
    };
  }
}
