import { defaultConfig } from './config/defaultConfig.js';
import { RalphLoops } from './core/ralphLoops.js';
import {
  ModelProviderRegistry,
  createCloudProvider,
  createLmStudioProvider,
  createOllamaProvider,
} from './core/modelProviders.js';
import { BuiltInRenderer } from './core/rendering.js';
import { ExtensionManager } from './core/extensionManager.js';

/**
 * Ralph IDE 부트스트랩입니다. / Ralph IDE bootstrap.
 * Fast Mode 중심 아키텍처와 핵심 내장 기능을 데모로 실행합니다.
 * Runs a demo for the Fast Mode-centered architecture and built-in capabilities.
 */
async function main() {
  const providers = new ModelProviderRegistry();
  providers.register(createOllamaProvider());
  providers.register(createLmStudioProvider());
  providers.register(
    createCloudProvider({
      endpoint: 'https://api.example.com/v1/chat/completions',
    }),
  );

  const renderer = new BuiltInRenderer();
  const extensions = new ExtensionManager({
    registryUrl: defaultConfig.extensions.openVsx.registryUrl,
  });

  const loops = new RalphLoops(defaultConfig.ralphLoops);
  const result = await loops.run({
    onInspect: async (i) => console.log(`[Loop ${i}] Inspect / 점검`),
    onBuild: async (i) => console.log(`[Loop ${i}] Build / 구현`),
    onAudit: async (i) => console.log(`[Loop ${i}] Audit / 감사`),
    onReview: async (i) => console.log(`[Loop ${i}] Review / 리뷰`),
    // 데모 안정성을 위해 2회 후 종료합니다.
    // Stops after 2 cycles for demo stability.
    stopSignal: (() => {
      let counter = 0;
      return () => {
        counter += 1;
        return counter > 2;
      };
    })(),
  });

  console.log('Mode / 모드:', defaultConfig.mode);
  console.log('Providers / 공급자:', providers.list());
  console.log('Open VSX URL:', extensions.getOpenVsxSearchUrl('python'));
  console.log('Import extension / 확장 파일 가져오기:', extensions.importExtensionFile('sample.vsix'));
  console.log('Markdown preview / 미리보기:', renderer.renderMarkdown('# Hello **Ralph IDE**'));
  console.log('PDF preview / 미리보기:', renderer.renderPdf('./docs/spec.pdf'));
  console.log('Loop result / 루프 결과:', result);
}

main().catch((error) => {
  console.error('Bootstrap failed / 부트스트랩 실패:', error);
  process.exitCode = 1;
});
