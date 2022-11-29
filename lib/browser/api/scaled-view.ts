import { BaseView } from 'electron/main';
import type { ScaledView as SVT } from 'electron/main';
const { ScaledView } = process._linkedBinding('electron_browser_scaled_view') as { ScaledView: typeof SVT };

Object.setPrototypeOf(ScaledView.prototype, BaseView.prototype);

const isScaledView = (view: any) => {
  return view && view.constructor.name === 'ScaledView';
};

ScaledView.fromId = (id: number) => {
  const view = BaseView.fromId(id);
  return isScaledView(view) ? view as any as SVT : null;
};

ScaledView.getAllViews = () => {
  return BaseView.getAllViews().filter(isScaledView) as any[] as SVT[];
};

module.exports = ScaledView;
