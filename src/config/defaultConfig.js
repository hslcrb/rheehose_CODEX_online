/**
 * 기본 IDE 설정입니다. / Default IDE configuration.
 * Fast Mode를 기본 활성화하고 Ralph Loops 정책을 정의합니다.
 * Enables Fast Mode by default and defines Ralph Loops policy.
 */
export const defaultConfig = {
  mode: 'fast',
  ralphLoops: {
    enabled: true,
    iterations: 'infinite',
    enforceAudit: true,
    enforceReview: true,
  },
  rendering: {
    markdown: {
      builtIn: true,
    },
    pdf: {
      builtIn: true,
      strategy: 'native-or-pdfjs',
    },
  },
  extensions: {
    openVsx: {
      enabled: true,
      registryUrl: 'https://open-vsx.org',
    },
    directImport: {
      enabled: true,
      acceptedFormats: ['.vsix', '.zip'],
    },
  },
  models: {
    local: ['ollama', 'lmstudio'],
    cloud: ['openai-compatible'],
  },
};
