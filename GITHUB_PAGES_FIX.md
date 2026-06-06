# GitHub Pages 404 修復指南

若開啟 https://singpraise.github.io/snake-game-final/ 顯示：

> **404 — There isn't a GitHub Pages site here.**

代表 **Pages 尚未部署成功**，請依序檢查：

---

## ① 確認 Repository 存在

能開啟（不是 404）：

https://github.com/singpraise/snake-game-final

---

## ② 開啟 GitHub Pages（必做）

1. 開啟：https://github.com/singpraise/snake-game-final/settings/pages
2. **Build and deployment → Source** 選 **GitHub Actions**（不是 Deploy from a branch）
3. 按 Save（若有）

---

## ③ 確認 Actions 有跑

1. 開啟：https://github.com/singpraise/snake-game-final/actions
2. 應看到 **Build and Deploy Web Game**
3. 狀態需為 **綠色 ✓**

若 **紅色 ✗** 失敗：點進去看錯誤 log。

若 **沒有任何 workflow**：代表 `.github/workflows/` 沒 push 上去，請在本機再 push 一次。

---

## ④ 手動觸發部署

1. Actions → **Build and Deploy Web Game**
2. 右側 **Run workflow** → **Run workflow**

等 2～5 分鐘後重新整理網頁。

---

## ⑤ 本機更新並 push（Git Bash）

```bash
cd "/c/Users/Lenovo/Documents/C_Joyce Wan/snake"
git add .
git commit -m "Fix GitHub Pages deployment workflow"
git push origin main
```

push 後 Actions 會自動再跑一次。

---

## 成功後

- 網址：https://singpraise.github.io/snake-game-final/
- Settings → Pages 會顯示：`Your site is live at ...`
