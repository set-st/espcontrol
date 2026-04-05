<template>
  <div class="icon-gallery">
    <input
      v-model="search"
      type="text"
      class="icon-search"
      placeholder="Search icons…"
    />
    <p class="icon-count">{{ filtered.length }} icon{{ filtered.length === 1 ? '' : 's' }}</p>
    <div class="icon-grid">
      <button
        v-for="icon in filtered"
        :key="icon.name"
        class="icon-card"
        :class="{ copied: copiedName === icon.name }"
        @click="copy(icon.name)"
      >
        <span class="icon-preview mdi" :class="'mdi-' + icon.mdi" />
        <span class="icon-name">{{ copiedName === icon.name ? 'Copied!' : icon.name }}</span>
      </button>
    </div>
  </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue'
import iconsData from '../../../../common/assets/icons.json'

const icons = iconsData.icons
const search = ref('')
const copiedName = ref(null)
let copyTimer = null

const filtered = computed(() => {
  const q = search.value.toLowerCase()
  if (!q) return icons
  return icons.filter(i => i.name.toLowerCase().includes(q))
})

function copy(name) {
  navigator.clipboard.writeText(name)
  copiedName.value = name
  clearTimeout(copyTimer)
  copyTimer = setTimeout(() => { copiedName.value = null }, 1200)
}

onMounted(() => {
  if (document.querySelector('link[href*="materialdesignicons"]')) return
  const link = document.createElement('link')
  link.rel = 'stylesheet'
  link.href = 'https://cdn.jsdelivr.net/npm/@mdi/font@7.4.47/css/materialdesignicons.min.css'
  document.head.appendChild(link)
})
</script>

<style scoped>
.icon-gallery {
  margin-top: 1rem;
}

.icon-search {
  width: 100%;
  padding: 10px 14px;
  font-size: 15px;
  border: 1px solid var(--vp-c-border);
  border-radius: 8px;
  background: var(--vp-c-bg-soft);
  color: var(--vp-c-text-1);
  outline: none;
  transition: border-color 0.2s;
  font-family: inherit;
}

.icon-search:focus {
  border-color: var(--vp-c-brand-1);
}

.icon-count {
  font-size: 13px;
  color: var(--vp-c-text-3);
  margin: 8px 0 12px;
}

.icon-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(120px, 1fr));
  gap: 10px;
}

.icon-card {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 6px;
  padding: 14px 8px 10px;
  border: 1px solid var(--vp-c-border);
  border-radius: 8px;
  background: var(--vp-c-bg-soft);
  cursor: pointer;
  transition: border-color 0.2s, transform 0.15s, background-color 0.2s;
  font-family: inherit;
  color: var(--vp-c-text-1);
}

.icon-card:hover {
  border-color: var(--vp-c-brand-1);
  transform: translateY(-2px);
}

.icon-card.copied {
  border-color: var(--vp-c-green-2);
  background: var(--vp-c-green-soft);
}

.icon-preview {
  font-size: 32px;
  line-height: 1;
  color: var(--vp-c-text-1);
}

.icon-name {
  font-size: 11px;
  text-align: center;
  line-height: 1.3;
  color: var(--vp-c-text-2);
  word-break: break-word;
}

.icon-card.copied .icon-name {
  color: var(--vp-c-green-2);
  font-weight: 600;
}
</style>
