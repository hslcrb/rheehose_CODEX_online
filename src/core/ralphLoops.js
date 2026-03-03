/**
 * Ralph Loops 엔진입니다. / Ralph Loops engine.
 * 반복 실행 시 점검, 감사, 리뷰를 소프트웨어 레벨에서 강제합니다.
 * Enforces inspection, audit, and review at software level during iterative execution.
 */
export class RalphLoops {
  constructor(options = {}) {
    this.iterations = options.iterations ?? 'infinite';
    this.enforceAudit = options.enforceAudit ?? true;
    this.enforceReview = options.enforceReview ?? true;
  }

  /**
   * 무한 또는 유한 반복 여부를 판별합니다.
   * Resolves whether looping should continue for finite/infinite settings.
   */
  shouldContinue(index) {
    if (this.iterations === 'infinite') return true;
    return index < Number(this.iterations);
  }

  /**
   * 반복 오케스트레이션을 실행합니다.
   * Executes orchestration over iterative cycles.
   */
  async run({ onInspect, onBuild, onAudit, onReview, stopSignal }) {
    let i = 0;
    while (this.shouldContinue(i)) {
      if (stopSignal?.()) break;

      await onInspect?.(i);
      await onBuild?.(i);

      if (this.enforceAudit) {
        await onAudit?.(i);
      }
      if (this.enforceReview) {
        await onReview?.(i);
      }

      i += 1;
    }

    return { completedIterations: i, mode: this.iterations };
  }
}
