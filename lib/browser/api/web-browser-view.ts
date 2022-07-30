import { BaseView } from 'electron/main';
import type { WebBrowserView as WBVT } from 'electron/main';
const { WebBrowserView } = process._linkedBinding('electron_browser_web_browser_view') as { WebBrowserView: typeof WBVT };

Object.setPrototypeOf(WebBrowserView.prototype, BaseView.prototype);

module.exports = WebBrowserView;
