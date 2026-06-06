"""
Flask 後端 — 路由與 Web API
整合 C/WebAssembly 貪食蛇：關卡管理、伺服器端排行榜、首頁渲染
"""
from __future__ import annotations

import json
import os
from datetime import datetime, timezone

from flask import Flask, jsonify, render_template, request

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
WEB_DIR = os.path.join(BASE_DIR, "web")
LEVELS_DIR = os.path.join(BASE_DIR, "levels")
DATA_DIR = os.path.join(BASE_DIR, "data")
SCORES_FILE = os.path.join(DATA_DIR, "highscores.json")
MAX_SCORES = 20

app = Flask(
    __name__,
    template_folder=os.path.join(BASE_DIR, "templates"),
    static_folder=WEB_DIR,
    static_url_path="",
)


def _ensure_data_dir() -> None:
    os.makedirs(DATA_DIR, exist_ok=True)
    if not os.path.isfile(SCORES_FILE):
        with open(SCORES_FILE, "w", encoding="utf-8") as fp:
            json.dump({"scores": []}, fp, ensure_ascii=False, indent=2)


def _load_scores() -> list[dict]:
    _ensure_data_dir()
    with open(SCORES_FILE, "r", encoding="utf-8") as fp:
        data = json.load(fp)
    return data.get("scores", [])


def _save_scores(scores: list[dict]) -> None:
    _ensure_data_dir()
    with open(SCORES_FILE, "w", encoding="utf-8") as fp:
        json.dump({"scores": scores}, fp, ensure_ascii=False, indent=2)


def _level_title(path: str) -> str:
    try:
        with open(path, "r", encoding="utf-8") as fp:
            for line in fp:
                if line.startswith("name="):
                    return line[5:].strip()
    except OSError:
        pass
    return os.path.basename(path)


def _safe_level_name(name: str) -> str | None:
    if not name.endswith(".lvl"):
        name = f"{name}.lvl"
    base = os.path.basename(name)
    if base != name or ".." in base:
        return None
    return base


@app.route("/")
def index():
    """首頁：渲染遊戲介面（WASM 由 static 提供）"""
    return render_template("index.html")


@app.route("/api/health")
def api_health():
    return jsonify(
        {
            "status": "ok",
            "backend": "Flask",
            "game_engine": "C WebAssembly",
            "version": "final",
        }
    )


@app.route("/api/levels", methods=["GET"])
def api_levels_list():
    """列出所有自訂關卡"""
    if not os.path.isdir(LEVELS_DIR):
        return jsonify({"levels": []})

    levels = []
    for name in sorted(os.listdir(LEVELS_DIR)):
        if not name.endswith(".lvl"):
            continue
        path = os.path.join(LEVELS_DIR, name)
        levels.append(
            {
                "name": name,
                "title": _level_title(path),
                "size": os.path.getsize(path),
            }
        )
    return jsonify({"levels": levels})


@app.route("/api/levels/<path:name>", methods=["GET"])
def api_levels_get(name: str):
    """取得關卡文字內容（供 WASM 載入）"""
    safe = _safe_level_name(name)
    if safe is None:
        return jsonify({"error": "invalid level name"}), 400

    path = os.path.join(LEVELS_DIR, safe)
    if not os.path.isfile(path):
        return jsonify({"error": "level not found"}), 404

    with open(path, "r", encoding="utf-8") as fp:
        content = fp.read()

    return jsonify({"name": safe, "content": content})


@app.route("/api/levels/<path:name>", methods=["POST"])
def api_levels_save(name: str):
    """儲存關卡（編輯器 / 管理用）"""
    safe = _safe_level_name(name)
    if safe is None:
        return jsonify({"error": "invalid level name"}), 400

    payload = request.get_json(silent=True) or {}
    content = payload.get("content")
    if not content or not isinstance(content, str):
        return jsonify({"error": "content required"}), 400

    os.makedirs(LEVELS_DIR, exist_ok=True)
    path = os.path.join(LEVELS_DIR, safe)
    with open(path, "w", encoding="utf-8") as fp:
        fp.write(content)

    return jsonify({"ok": True, "name": safe})


@app.route("/api/scores", methods=["GET"])
def api_scores_get():
    """取得排行榜（依分數降序）"""
    scores = sorted(_load_scores(), key=lambda s: s.get("score", 0), reverse=True)
    return jsonify({"scores": scores[:MAX_SCORES]})


@app.route("/api/scores", methods=["POST"])
def api_scores_post():
    """提交分數到伺服器"""
    payload = request.get_json(silent=True) or {}
    player = str(payload.get("player", "Player")).strip()[:32] or "Player"
    score = payload.get("score")
    mode = str(payload.get("mode", "enhanced")).strip()[:32]

    if not isinstance(score, int) or score < 0:
        return jsonify({"error": "invalid score"}), 400

    entry = {
        "player": player,
        "score": score,
        "mode": mode,
        "time": datetime.now(timezone.utc).isoformat(),
    }

    scores = _load_scores()
    scores.append(entry)
    scores = sorted(scores, key=lambda s: s.get("score", 0), reverse=True)[:MAX_SCORES]
    _save_scores(scores)

    return jsonify({"ok": True, "entry": entry, "rank": scores.index(entry) + 1})


if __name__ == "__main__":
    _ensure_data_dir()
    print("C Snake Game Final — Flask server")
    print("Open http://127.0.0.1:5000")
    app.run(host="127.0.0.1", port=5000, debug=True)
