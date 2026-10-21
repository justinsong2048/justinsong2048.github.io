// Justin Song portfolio · site.js
// Menu toggle, figure lightbox, Work filter, copy-email buttons.
(function () {
  "use strict";

  // Phone menu ------------------------------------------------
  const toggle = document.querySelector(".nav-toggle");
  const nav = document.getElementById("site-nav");
  if (toggle && nav) {
    const setOpen = (open) => {
      toggle.setAttribute("aria-expanded", String(open));
      toggle.textContent = open ? "Close" : "Menu";
      nav.classList.toggle("is-open", open);
    };
    toggle.addEventListener("click", () => setOpen(toggle.getAttribute("aria-expanded") !== "true"));
    nav.addEventListener("click", (e) => { if (e.target.closest("a")) setOpen(false); });
    document.addEventListener("keydown", (e) => {
      if (e.key === "Escape" && toggle.getAttribute("aria-expanded") === "true") { setOpen(false); toggle.focus(); }
    });
  }

  // Lightbox --------------------------------------------------
  const triggers = Array.from(document.querySelectorAll("[data-lightbox]"));
  if (triggers.length) {
    const box = document.createElement("div");
    box.className = "lightbox";
    box.hidden = true;
    box.setAttribute("role", "dialog");
    box.setAttribute("aria-modal", "true");
    box.setAttribute("aria-label", "Figure viewer");
    box.innerHTML =
      '<div class="lightbox-bar"><span class="label" data-count></span>' +
      '<span><button type="button" data-prev>← Prev</button> <button type="button" data-next>Next →</button> ' +
      '<button type="button" data-close>Close</button></span></div>' +
      '<div class="lightbox-stage"><img alt=""></div><p class="lightbox-caption"></p>';
    document.body.appendChild(box);
    const img = box.querySelector("img");
    const cap = box.querySelector(".lightbox-caption");
    const count = box.querySelector("[data-count]");
    let index = 0;
    let opener = null;

    const show = (i) => {
      index = (i + triggers.length) % triggers.length;
      const t = triggers[index];
      img.src = t.dataset.lightbox;
      img.alt = t.querySelector("img") ? t.querySelector("img").alt : "";
      cap.textContent = t.dataset.caption || "";
      count.textContent = "Figure " + (index + 1) + " of " + triggers.length;
    };
    const open = (i, from) => {
      opener = from;
      show(i);
      box.hidden = false;
      document.body.style.overflow = "hidden";
      box.querySelector("[data-close]").focus();
    };
    const close = () => {
      box.hidden = true;
      document.body.style.overflow = "";
      if (opener) opener.focus();
    };

    triggers.forEach((t, i) => t.addEventListener("click", () => open(i, t)));
    box.querySelector("[data-prev]").addEventListener("click", () => show(index - 1));
    box.querySelector("[data-next]").addEventListener("click", () => show(index + 1));
    box.querySelector("[data-close]").addEventListener("click", close);
    box.addEventListener("click", (e) => { if (e.target === box || e.target.classList.contains("lightbox-stage")) close(); });
    document.addEventListener("keydown", (e) => {
      if (box.hidden) return;
      if (e.key === "Escape") close();
      if (e.key === "ArrowLeft") show(index - 1);
      if (e.key === "ArrowRight") show(index + 1);
    });
    if (triggers.length < 2) {
      box.querySelector("[data-prev]").hidden = true;
      box.querySelector("[data-next]").hidden = true;
    }
  }

  // Work filter -----------------------------------------------
  const bar = document.querySelector(".filter-bar");
  if (bar) {
    const cards = Array.from(document.querySelectorAll(".project-card[data-tags]"));
    bar.hidden = false;
    bar.addEventListener("click", (e) => {
      const btn = e.target.closest("button[data-filter]");
      if (!btn) return;
      bar.querySelectorAll("button").forEach((b) => b.setAttribute("aria-pressed", String(b === btn)));
      const f = btn.dataset.filter;
      cards.forEach((c) => { c.hidden = f !== "all" && !c.dataset.tags.split(" ").includes(f); });
    });
  }

  // Copy email ------------------------------------------------
  document.querySelectorAll("[data-copy]").forEach((btn) => {
    const status = document.getElementById(btn.getAttribute("aria-describedby") || "");
    btn.addEventListener("click", async () => {
      const text = btn.dataset.copy;
      try {
        await navigator.clipboard.writeText(text);
        if (status) status.textContent = "Copied " + text;
      } catch (err) {
        if (status) status.textContent = "Copy didn't work here. Select the address and copy it.";
      }
    });
  });
})();
