/**
 * Flask 後端 API 橋接（關卡、排行榜）
 * 僅在透過 Flask 執行時啟用（/api/* 可用）
 */
(function () {
  var submittedForRound = false;

  function apiBase() {
    return window.location.origin;
  }

  function fetchJson(url, options) {
    return fetch(url, options).then(function (res) {
      if (!res.ok) {
        throw new Error('API ' + res.status);
      }
      return res.json();
    });
  }

  function renderScores(scores) {
    var list = document.getElementById('server-scores');
    if (!list) return;

    list.innerHTML = '';
    if (!scores || scores.length === 0) {
      list.innerHTML = '<li>No scores yet</li>';
      return;
    }

    scores.forEach(function (row, i) {
      var li = document.createElement('li');
      li.textContent = (i + 1) + '. ' + row.player + ' — ' + row.score +
        ' (' + row.mode + ')';
      list.appendChild(li);
    });
  }

  window.flaskApi = {
    enabled: false,

    init: function () {
      return fetchJson(apiBase() + '/api/health')
        .then(function () {
          window.flaskApi.enabled = true;
          var badge = document.getElementById('flask-badge');
          if (badge) badge.textContent = 'Flask API Online';
          return Promise.all([
            window.flaskApi.refreshScores(),
            window.flaskApi.refreshLevels()
          ]);
        })
        .catch(function () {
          var badge = document.getElementById('flask-badge');
          if (badge) badge.textContent = 'Static mode (no Flask)';
        });
    },

    refreshScores: function () {
      if (!window.flaskApi.enabled) return Promise.resolve();
      return fetchJson(apiBase() + '/api/scores').then(function (data) {
        renderScores(data.scores || []);
      });
    },

    refreshLevels: function () {
      if (!window.flaskApi.enabled) return Promise.resolve();
      return fetchJson(apiBase() + '/api/levels').then(function (data) {
        var select = document.getElementById('level-select');
        if (!select) return;
        select.innerHTML = '';
        (data.levels || []).forEach(function (lv) {
          var opt = document.createElement('option');
          opt.value = lv.name;
          opt.textContent = lv.title ? lv.title + ' (' + lv.name + ')' : lv.name;
          select.appendChild(opt);
        });
      });
    },

    loadSelectedLevel: function () {
      if (!window.flaskApi.enabled || !Module._web_load_level_text) {
        return;
      }
      var select = document.getElementById('level-select');
      if (!select || !select.value) return;

      fetchJson(apiBase() + '/api/levels/' + encodeURIComponent(select.value))
        .then(function (data) {
          var ok = Module._web_load_level_text(data.content);
          if (ok !== 0) {
            alert('Failed to load level');
            return;
          }
          Module.setStatus('Custom: ' + data.name);
          submittedForRound = false;
        })
        .catch(function () {
          alert('Level load failed');
        });
    },

    fetchLevelToEditor: function () {
      if (!window.flaskApi.enabled) return Promise.resolve();
      var select = document.getElementById('level-select');
      var textarea = document.getElementById('level-content');
      var filename = document.getElementById('level-filename');
      if (!select || !select.value || !textarea) return Promise.resolve();

      return fetchJson(apiBase() + '/api/levels/' + encodeURIComponent(select.value))
        .then(function (data) {
          textarea.value = data.content || '';
          if (filename) filename.value = data.name || select.value;
        })
        .catch(function () {
          alert('Failed to load level text');
        });
    },

    saveLevelFromEditor: function () {
      if (!window.flaskApi.enabled) return;
      var textarea = document.getElementById('level-content');
      var filename = document.getElementById('level-filename');
      if (!textarea || !filename) return;

      var name = filename.value.trim();
      if (!name) {
        alert('Enter a level filename');
        return;
      }

      fetchJson(apiBase() + '/api/levels/' + encodeURIComponent(name), {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ content: textarea.value })
      }).then(function () {
        alert('Level saved: ' + name);
        return window.flaskApi.refreshLevels();
      }).catch(function () {
        alert('Failed to save level');
      });
    },

    submitScore: function () {
      if (!window.flaskApi.enabled || !Module._web_get_score) return;

      var playerEl = document.getElementById('player-name');
      var player = playerEl ? playerEl.value.trim() : 'Player';
      if (!player) player = 'Player';

      var score = Module._web_get_score();
      var modeEl = document.querySelector('input[name="mode"]:checked');
      var mode = modeEl ? modeEl.value : 'enhanced';

      fetchJson(apiBase() + '/api/scores', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ player: player, score: score, mode: mode })
      }).then(function (data) {
        alert('Score saved! Rank #' + data.rank);
        return window.flaskApi.refreshScores();
      }).catch(function () {
        alert('Failed to save score');
      });
    },

    pollGameOver: function () {
      if (!window.flaskApi.enabled || !Module._web_is_game_over) return;
      if (!Module._web_is_game_over()) {
        submittedForRound = false;
        return;
      }
      if (submittedForRound) return;
      submittedForRound = true;

      var score = Module._web_get_score ? Module._web_get_score() : 0;
      var badge = document.getElementById('status-badge');
      if (badge) {
        badge.textContent = 'Game Over — Score ' + score;
      }
    }
  };

  setInterval(function () {
    if (window.flaskApi && window.flaskApi.enabled) {
      window.flaskApi.pollGameOver();
    }
  }, 500);

  document.addEventListener('DOMContentLoaded', function () {
    var loadBtn = document.getElementById('btn-load-level');
    var saveBtn = document.getElementById('btn-submit-score');
    if (loadBtn) {
      loadBtn.addEventListener('click', function () {
        window.flaskApi.loadSelectedLevel();
      });
    }
    if (saveBtn) {
      saveBtn.addEventListener('click', function () {
        window.flaskApi.submitScore();
      });
    }
    var fetchBtn = document.getElementById('btn-fetch-level');
    var saveLevelBtn = document.getElementById('btn-save-level');
    if (fetchBtn) {
      fetchBtn.addEventListener('click', function () {
        window.flaskApi.fetchLevelToEditor();
      });
    }
    if (saveLevelBtn) {
      saveLevelBtn.addEventListener('click', function () {
        window.flaskApi.saveLevelFromEditor();
      });
    }
    window.flaskApi.init();
  });
})();
