import random
import threading
import tkinter as tk
from pathlib import Path
from tkinter import filedialog, messagebox

from pynput import keyboard


class TypeAssistApp:
    def __init__(self, root: tk.Tk) -> None:
        self.root = root
        self.root.title("원천텍스트 타이핑 도우미")
        self.root.geometry("720x320")

        self.source_path: Path | None = None
        self.target_path: Path | None = None
        self.source_text = ""
        self.remaining_text = ""
        self.recording = False

        self.listener: keyboard.Listener | None = None
        self.pressed_modifiers: set[keyboard.Key] = set()
        self.file_lock = threading.Lock()

        self._build_ui()
        self._start_keyboard_listener()

    def _build_ui(self) -> None:
        frame = tk.Frame(self.root, padx=12, pady=12)
        frame.pack(fill="both", expand=True)

        tk.Label(
            frame,
            text="원천텍스트(.txt/.py/.jsp 등 텍스트 파일)와 비어있는 대상텍스트를 지정하세요.",
            anchor="w",
        ).pack(fill="x")

        self.source_var = tk.StringVar(value="원천텍스트: 미선택")
        self.target_var = tk.StringVar(value="대상텍스트: 미선택")

        source_row = tk.Frame(frame)
        source_row.pack(fill="x", pady=(10, 6))
        tk.Label(source_row, textvariable=self.source_var, anchor="w").pack(side="left", fill="x", expand=True)
        tk.Button(source_row, text="원천텍스트 선택", command=self.choose_source).pack(side="right")

        target_row = tk.Frame(frame)
        target_row.pack(fill="x", pady=6)
        tk.Label(target_row, textvariable=self.target_var, anchor="w").pack(side="left", fill="x", expand=True)
        tk.Button(target_row, text="대상텍스트 선택", command=self.choose_target).pack(side="right")

        self.status_var = tk.StringVar(value="상태: 준비")
        tk.Label(frame, textvariable=self.status_var, font=("Arial", 12, "bold")).pack(fill="x", pady=(12, 6))

        self.progress_var = tk.StringVar(value="남은 글자 수: -")
        tk.Label(frame, textvariable=self.progress_var).pack(fill="x", pady=(0, 12))

        self.start_btn = tk.Button(
            frame,
            text="START",
            state="disabled",
            width=18,
            height=2,
            bg="#d9d9d9",
            command=self.toggle_recording,
        )
        self.start_btn.pack()

        tk.Label(
            frame,
            fg="#555",
            justify="left",
            text=(
                "사용법:\n"
                "1) START를 눌러 REC로 바꿉니다.\n"
                "2) 외부 에디터에서 키보드를 칠 때마다 원천텍스트가 1~5자 랜덤으로 대상파일에 기록됩니다.\n"
                "3) 다시 버튼을 눌러 중지합니다."
            ),
        ).pack(fill="x", pady=(14, 0))

        self.root.protocol("WM_DELETE_WINDOW", self.on_close)

    def choose_source(self) -> None:
        selected = filedialog.askopenfilename(title="원천텍스트 선택")
        if not selected:
            return

        path = Path(selected)
        try:
            text = path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            try:
                text = path.read_text(encoding="cp949")
            except Exception as exc:
                messagebox.showerror("오류", f"원천텍스트를 읽지 못했습니다.\n{exc}")
                return
        except Exception as exc:
            messagebox.showerror("오류", f"원천텍스트를 읽지 못했습니다.\n{exc}")
            return

        if not text:
            messagebox.showwarning("경고", "원천텍스트가 비어 있습니다.")

        self.source_path = path
        self.source_text = text
        self.source_var.set(f"원천텍스트: {path}")
        self._validate_ready_state()

    def choose_target(self) -> None:
        selected = filedialog.askopenfilename(title="대상텍스트 선택")
        if not selected:
            return

        path = Path(selected)
        if not path.exists() or not path.is_file():
            messagebox.showerror("오류", "유효한 파일이 아닙니다.")
            return

        try:
            existing = path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            try:
                existing = path.read_text(encoding="cp949")
            except Exception:
                existing = path.read_bytes().decode("utf-8", errors="ignore")
        except Exception as exc:
            messagebox.showerror("오류", f"대상텍스트를 읽지 못했습니다.\n{exc}")
            return

        if existing:
            messagebox.showerror("오류", "대상텍스트는 완전히 비어 있어야 합니다.")
            return

        self.target_path = path
        self.target_var.set(f"대상텍스트: {path}")
        self._validate_ready_state()

    def _validate_ready_state(self) -> None:
        ready = self.source_path is not None and self.target_path is not None
        if ready:
            self.start_btn.configure(state="normal")
            self.status_var.set("상태: 준비 완료 (START 가능)")
            self.progress_var.set(f"남은 글자 수: {len(self.source_text)}")
        else:
            self.start_btn.configure(state="disabled")
            self.status_var.set("상태: 준비")
            self.progress_var.set("남은 글자 수: -")

    def toggle_recording(self) -> None:
        if not self.recording:
            if self.source_path is None or self.target_path is None:
                return

            self.remaining_text = self.source_text
            self.recording = True
            self.start_btn.configure(text="REC", bg="#ff3b30", activebackground="#ff3b30")
            self.status_var.set("상태: REC (작동중)")
            self.progress_var.set(f"남은 글자 수: {len(self.remaining_text)}")
            return

        self.recording = False
        self.start_btn.configure(text="START", bg="#d9d9d9", activebackground="#ececec")
        self.status_var.set("상태: 중지")

    def _start_keyboard_listener(self) -> None:
        self.listener = keyboard.Listener(on_press=self._on_key_press, on_release=self._on_key_release)
        self.listener.daemon = True
        self.listener.start()

    def _on_key_release(self, key) -> None:
        if key in (keyboard.Key.ctrl, keyboard.Key.ctrl_l, keyboard.Key.ctrl_r,
                   keyboard.Key.alt, keyboard.Key.alt_l, keyboard.Key.alt_r,
                   keyboard.Key.cmd, keyboard.Key.cmd_l, keyboard.Key.cmd_r,
                   keyboard.Key.shift, keyboard.Key.shift_l, keyboard.Key.shift_r):
            self.pressed_modifiers.discard(key)

    def _on_key_press(self, key) -> None:
        if key in (keyboard.Key.ctrl, keyboard.Key.ctrl_l, keyboard.Key.ctrl_r,
                   keyboard.Key.alt, keyboard.Key.alt_l, keyboard.Key.alt_r,
                   keyboard.Key.cmd, keyboard.Key.cmd_l, keyboard.Key.cmd_r,
                   keyboard.Key.shift, keyboard.Key.shift_l, keyboard.Key.shift_r):
            self.pressed_modifiers.add(key)
            return

        if not self.recording or self.target_path is None or not self.remaining_text:
            return

        if self.pressed_modifiers:
            return

        if not self._is_plain_typing_key(key):
            return

        chunk_size = random.randint(1, 5)
        chunk = self.remaining_text[:chunk_size]
        self.remaining_text = self.remaining_text[len(chunk):]

        with self.file_lock:
            self.target_path.write_text(self.target_path.read_text(encoding="utf-8", errors="ignore") + chunk, encoding="utf-8")

        self.root.after(0, self._update_progress)

    def _is_plain_typing_key(self, key) -> bool:
        if isinstance(key, keyboard.KeyCode):
            return key.char is not None
        return key in (keyboard.Key.space, keyboard.Key.enter, keyboard.Key.tab)

    def _update_progress(self) -> None:
        self.progress_var.set(f"남은 글자 수: {len(self.remaining_text)}")
        if not self.remaining_text and self.recording:
            self.recording = False
            self.start_btn.configure(text="START", bg="#d9d9d9", activebackground="#ececec")
            self.status_var.set("상태: 완료 (원천텍스트 전부 기록됨)")
            messagebox.showinfo("완료", "원천텍스트 전체가 대상텍스트에 기록되었습니다.")

    def on_close(self) -> None:
        if self.listener is not None:
            self.listener.stop()
        self.root.destroy()


if __name__ == "__main__":
    root = tk.Tk()
    app = TypeAssistApp(root)
    root.mainloop()
