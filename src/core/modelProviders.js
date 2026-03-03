/**
 * LLM 공급자 추상화 계층입니다. / LLM provider abstraction layer.
 * 로컬(Ollama/LM Studio) 및 클라우드(API Key) 모델을 동일 인터페이스로 제공합니다.
 * Offers a unified interface for local (Ollama/LM Studio) and cloud (API key) models.
 */
export class ModelProviderRegistry {
  constructor() {
    this.providers = new Map();
  }

  register(provider) {
    this.providers.set(provider.id, provider);
  }

  get(providerId) {
    return this.providers.get(providerId);
  }

  list() {
    return Array.from(this.providers.keys());
  }
}

export function createOllamaProvider(baseUrl = 'http://127.0.0.1:11434') {
  return {
    id: 'ollama',
    type: 'local',
    baseUrl,
  };
}

export function createLmStudioProvider(baseUrl = 'http://127.0.0.1:1234') {
  return {
    id: 'lmstudio',
    type: 'local',
    baseUrl,
  };
}

export function createCloudProvider({ id = 'openai-compatible', apiKeyEnv = 'MODEL_API_KEY', endpoint }) {
  return {
    id,
    type: 'cloud',
    apiKeyEnv,
    endpoint,
  };
}
