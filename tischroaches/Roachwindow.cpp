#include "tisch.h"
#include "Window.h"
#include <BlobDim.h>
#include <BlobPos.h>
#include <BlobID.h>
#include <vector>
#include <map>

class Roachwindow: public Window {

	public:

		Roachwindow( int width, int height, const char* name, int use_mouse ):
			Window(width,height,name,use_mouse)
		{
			shadowchanged = false;
			counter = 0;
			Gesture shadow( "shadow" );
			shadow.push_back( new BlobID()  );
			shadow.push_back( new BlobDim() );
			shadow.push_back( new BlobPos() );

			region.gestures.push_back( shadow );

			region.flags( 1<<INPUT_TYPE_SHADOW );

			std::vector<int> bounds; bounds.push_back( -1 );
			BlobID* gone = new BlobID(1<<INPUT_TYPE_SHADOW);
			gone->bounds( bounds );
			Gesture vanish( "vanish" );
			vanish.push_back( gone );
			region.gestures.push_back(vanish);
		}

		Vector shadowPos;
		Dimensions shadowDim;
		int shadowID;
		bool shadowchanged;
		bool shadowerased;
		int counter;

		void action( Gesture* gesture ) {
//			std::cout << "got action: " << gesture->name() << std::endl;
			
			if (gesture->name() == "shadow" ) {
//				std::cout << "Size: " << gesture->size() << std::endl;
				FeatureBase* f = (*gesture)[0];
				BlobID* m = dynamic_cast<BlobID*>(f);
				if (!m) return;
				int tmpID = m->result();
//				std::cout << "ID: " << tmp << std::endl;
				shadowID = tmpID;
					
				f = (*gesture)[1];
				BlobDim* n = dynamic_cast<BlobDim*>(f);
				if (!n) return;
				Dimensions tmp2 = n->result();
				shadowDim = tmp2;
//				std::cout << tmp2 << std::endl;

				f = (*gesture)[2];
				BlobPos* o = dynamic_cast<BlobPos*>(f);
				if (!o) return;
				Vector tmp = o->result();
//				transform( tmp, 1 );
//				std::cout << "Pos: " << tmp << std::endl;
				shadowPos = tmp;
				//std::cout << "shadowid: " << m->result() << std::endl;
				shadowchanged = true;
			}
			if (gesture->name() == "vanish") {
				FeatureBase* f = (*gesture)[0];
				BlobID* i = dynamic_cast<BlobID*>(f);
				if (!i) { std::cout << "FAIL" << std::endl; return; }
				int tmp2 = i->result();
				shadowID = tmp2;
				//std::cout << "vanish: " << i->result() << std::endl;
				shadowerased = true;
			}
		}
};

