#ifndef SCGRAPH_UI_POOL_HH
#define SCGRAPH_UI_POOL_HH

#include <QtUiTools/QUiLoader>

#include <vector>


class UIPool {
	public:
		static UIPool* get_instance();

	protected:
		UIPool();
		std::vector<QUiLoader> _ui_loaders;

		static UIPool *_instance;

};


#endif

