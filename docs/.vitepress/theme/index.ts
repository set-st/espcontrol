import DefaultTheme from 'vitepress/theme'
import EspInstallButton from './components/EspInstallButton.vue'
import IconGallery from './components/IconGallery.vue'

export default {
  extends: DefaultTheme,
  enhanceApp({ app }) {
    app.component('EspInstallButton', EspInstallButton)
    app.component('IconGallery', IconGallery)
  },
}
