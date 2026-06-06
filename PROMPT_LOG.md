# Cursor 協作紀錄與除錯歷程（Prompt Log）

> 本文件記錄使用 **Cursor AI** 開發 `snake-game-final` 的關鍵 Prompt、AI 的回應重點、以及**人工檢查與除錯**的過程。
>
> 核心原則（呼應評分標準）：**絕對不盲目複製貼上 AI 生成的程式碼，務必親自檢查每一行、實際編譯與執行驗證。**

---

## 一、開發階段與關鍵 Prompt

| 階段 | 主要 Prompt（摘要） | AI 產出 | 人工檢查重點 |
|------|--------------------|---------|--------------|
| 1. 架構規劃 | 「以 C 語言為導向設計貪食蛇，重點放在 struct、函式、指標與 malloc」 | 模組架構、`GameState` struct、鏈結串列蛇身 | 確認 struct 欄位合理、標頭檔不循環引用 |
| 2. 資料結構選型 | 「蛇身該用哪種資料結構？比較 Linked List / Array / Queue」 | 比較表 + 建議 Queue（FIFO） | 驗證移動是「新頭進、舊尾出」確實是 FIFO，非 Stack |
| 3. 核心移動 | 「用 Queue 實作完整移動邏輯與碰撞」 | `game_update()` 流程 | 逐行確認 enqueue/dequeue 時機、吃食物不 dequeue |
| 4. 2D Array 地圖 | 「加入 2D Array 讓碰撞與繪製 O(1)」 | `board.c` 二維陣列配置 | 確認 `board_get/set` 有邊界檢查 |
| 5. 記憶體安全 | 「強化 malloc/free 配對、避免 leak 與懸空指標」 | `memory.c`、`SAFE_FREE` | 用 `MEM_DEBUG` 檢查配置/釋放次數是否相等 |
| 6. 強化模式 | 「加障礙物、關卡、獎勵食物、最高分、四段速度」 | `game_config.c`、`obstacles.c`、`score.c` | 確認難度參數與升級邏輯 |
| 7. 音效 + 編輯器 | 「增加音效與關卡編輯器」 | `audio.c`、`editor.c`、`level.c` | 確認 Windows `Beep()` 與檔案存取邊界 |
| 8. 網頁版 | 「用 Emscripten 把 C 編譯成 WebAssembly，核心不改」 | `main_web.c`、`platform_web.c`、`web/` | 確認平台層抽象，核心 `.c` 不變 |
| 9. Flask 後端 | 「加 Python Flask 做路由與 Web 邏輯」 | `app.py`、REST API、模板 | 測試 `/api/health`、`/api/scores` 回應 |
| 10. GitHub Pages | 「自動部署到 GitHub Pages」 | GitHub Actions workflow | **見下方除錯歷程（重點）** |

---

## 二、GitHub Actions 除錯歷程（最佳「不可盲信 AI」實例）

GitHub Pages 自動部署一連串失敗 **14 次**才成功，過程完整示範了「AI 產出需逐步驗證、看真實錯誤訊息」的重要性。

| Run | 嘗試的修法 | 結果 | 真正的問題 |
|-----|-----------|------|-----------|
| #1–#3 | 手動 `emsdk install latest` | ❌ 30s 失敗 | Emscripten 安裝不穩定 |
| #4–#5 | 釘選版本 3.1.64 + cache | ❌ | `build_web.sh` 仍失敗 |
| #6–#7 | `setup-emsdk` action + 內嵌 emcc | ❌ | `emcc` 不在 PATH |
| #8 | 改用 `docker run emscripten/emsdk` | ❌ | docker 檔案權限/掛載 |
| #9–#11 | 換 `mymindstorm/setup-emsdk` | ❌ 0 秒失敗 | **`build_web.sh` 是 Windows CRLF 換行**，Linux bash 無法解析 |
| #12 | `container: emscripten/emsdk:3.1.64` | ❌ 0 秒失敗 | GitHub Actions **繞過映像 ENTRYPOINT**，`emcc` 仍不在 PATH |
| #13–#14 | `source $EMSDK/emsdk_env.sh` + 把 emcc 錯誤轉成 annotation | ❌ 但**終於看到真錯誤** | 兩個 C 編譯錯誤被環境問題掩蓋 |
| **#15** | 修掉兩個 C 錯誤 | ✅ **成功部署** | — |

### 最後兩個真正的 C 錯誤（被環境問題掩蓋）

```text
platform_web.c:70:5: error: EM_ASM does not work in -std=c* modes, use -std=gnu* modes instead
platform_web.c:61:30: error: duplicate case value '85'
```

**修正：**

1. **`EM_ASM` 不支援 `-std=c99`** → 編譯改用 `-std=gnu99`
2. **`case 'U'` 與 `case 85` 重複** → 因為字母 `'U'` 的 ASCII 正好是 **85**，兩者相同。移除多餘的 `case 85:`：

```c
/* 修正前（重複）*/
case 'u': case 'U': case 85: game_undo(game); break;
/* 修正後 */
case 'u': case 'U': game_undo(game); break;
```

### 除錯方法論（學到的關鍵）

1. **不要一直換工具亂猜** — 前 12 次都在猜環境，真正該做的是**讀錯誤 log**。
2. **讓錯誤可見** — 在 CI 把 `emcc` 的 stderr 轉成 GitHub annotation，才看到真正的編譯錯誤。
3. **分層定位** — 先確認 `emcc --version` 能跑（環境 OK），再看編譯錯誤（程式碼問題）。
4. **跨平台陷阱** — Windows 開發的 `.sh` 要強制 LF（`.gitattributes`），否則 Linux CI 會 0 秒崩潰。

---

## 三、對應「常見地雷」的防範（人工檢查成果）

| 常見地雷 | 本專案如何避免 | 對應程式 |
|----------|----------------|----------|
| Memory Leak（忘記 free） | 每個 `create` 都有對應 `destroy`，逐層釋放 | `game_destroy()`、`SAFE_FREE` |
| Segmentation Fault（指標亂指） | 配置後立即檢查 `NULL`，釋放後指標歸零 | `mem_malloc` 回傳檢查、`SAFE_FREE` |
| Array Out-of-Bounds（陣列越界） | 每次 `board[y][x]` 存取前做邊界檢查 | `board_get()` / `board_set()` |
| Allocation Errors（malloc 大小/型別錯） | 統一 `mem_malloc(sizeof(T))` / `mem_calloc` | `memory.c` |
| Missing Deallocation（忘記寫釋放邏輯） | 所有權規則表 + `MEM_DEBUG` 洩漏統計 | `memory.c` 計數器 |

---

## 四、結論

- AI（Cursor）大幅加速了**架構發想、樣板程式、文件撰寫**。
- 但**功能正確性、記憶體安全、跨平台部署**全靠**親自編譯、執行、讀錯誤訊息**逐項驗證。
- 最有價值的一課：**GitHub Actions 14 次失敗，靠的不是換更多工具，而是讓真正的錯誤訊息浮現、逐行檢查。**
