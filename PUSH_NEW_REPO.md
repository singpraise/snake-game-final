# 推送到新 Repository：`snake-game-final`

本資料夾是 **Final 完整版**，請推到**新 repo**，不要 push 到舊的 `snake-game`。

## 網址

| 項目 | URL |
|------|-----|
| 新 Repository | https://github.com/singpraise/snake-game-final |
| 網頁遊戲 | https://singpraise.github.io/snake-game-final/ |
| 舊版（不動） | https://github.com/singpraise/snake-game |

---

## 步驟一：在 GitHub 建立新 repo

1. 開啟 https://github.com/new
2. Repository name：**`snake-game-final`**
3. 選 **Public**
4. **不要**勾選 README / .gitignore（本機已有）
5. 按 **Create repository**

---

## 步驟二：本機 push（PowerShell）

```powershell
cd "c:\Users\Lenovo\Documents\C_Joyce Wan\snake"

# 若尚未 init
git init
git add .
git commit -m "C Snake Game Final: enhanced mode, editor, sound, 4 speed levels"

git branch -M main

# 只連到新 repo（不要設成 snake-game）
git remote remove origin 2>$null
git remote add origin https://github.com/singpraise/snake-game-final.git

git push -u origin main
```

若遠端已有 `origin` 指向舊 repo，務必先 `git remote remove origin` 再 add 新網址。

---

## 步驟三：開啟 GitHub Pages

1. https://github.com/singpraise/snake-game-final/settings/pages
2. **Source** → **GitHub Actions**
3. push 後等 Actions 跑完
4. 開啟 https://singpraise.github.io/snake-game-final/

---

## 速度難度（網頁版：簡單 / 中間 / 更難）

| 選項 | Classic 速度 | Enhanced 速度 |
|------|-------------|---------------|
| 簡單 | 150 ms/幀 | 140 ms/幀 |
| 中間 | 120 ms/幀 | 110 ms/幀 |
| 更難 | 90 ms/幀 | 85 ms/幀 |
| （Extreme，僅 C 核心保留） | 65 ms/幀 | 58 ms/幀 |

Enhanced 模式升級後會再加速。網頁 UI 已移除「最難（Extreme）」選項，C 核心仍保留該段速度設定。
