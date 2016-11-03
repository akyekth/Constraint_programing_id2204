/*      FOR 1 Solution of N=4
                      Queens.cpp                               NQueens.cpp
               
			       More Propagations                         less Propagations
    Constraints       icl function                            ICL_VAL             (For reduce Propagators Count)
	Branching         


*/


#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <iostream>

#if defined(GECODE_HAS_QT) && defined(GECODE_HAS_GIST)
#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#endif


using namespace Gecode;


class NQueens : public Script {
protected:
	IntVarArray q;
	const int n;
	int i,j;

public:

	NQueens(const SizeOptions& opt) : Script(opt), n(opt.size()), q(*this, opt.size() * opt.size(), 0, 1) {
		std::cout << "option size " << opt.size() << std::endl ;
		
	   // intialize matrix with zeros and once
		Matrix<IntVarArray> mat(q, n, n);

		
			for ((int)i = 0; i < n; i++)
			{
				// row constraint. when we add row in matix we will get 1
				linear(*this, mat.row(i), IRT_EQ, 1,ICL_VAL);
				//column constraint,when we add each column in matrix we get 1
				linear(*this, mat.col(i), IRT_EQ, 1,ICL_VAL);
			}

			//diagonal constraints,when we add diagonal elements always get <= 0 

			for ((int)i = 0; i < n; i++){

				IntVarArray dia(*this, n - i);
				for ((int)j = 0; i + j < n; j++) {
					dia[j] = mat(i + j, j);
				}
				linear(*this, dia, IRT_LQ, 1,ICL_VAL);
			}

			for ( (int)j = 0; j < n; ++j) {
				IntVarArray dia(*this, n - j);
				for ( (int)i = 0; i + j < n; ++i) {
					dia[i] = mat(i, i + j);
				}
				linear(*this, dia, IRT_LQ, 1,ICL_VAL);
			}

			for ((int)j = 0; j < n; ++j) {
				IntVarArray dia(*this, n - j);
				for ((int)i = 0; i + j < n; ++i) {
					dia[i] = mat((n - 1) - (i + j), i);
				}
				linear(*this, dia, IRT_LQ, 1,ICL_VAL);
			}

			for ((int) j = 0; j < n; ++j) {
				IntVarArray dia(*this, n - j);
				for ((int) i = 0; i + j < n; ++i) {
					dia[i] = mat((n - 1) - (i + 0), i + j);
				}
				linear(*this, dia, IRT_LQ, 1,ICL_VAL);
			}
		 
		
			/*Post Branching if we put INT_VAR_MAX_MAX , INT_VAR_MIN_MIN, INT_VAR_SIZE_MAX get same propagations. 
			  if we use Random generator in branching it gives some times less value as compared to before
			*/
		  
			Rnd r(2U);
			branch(*this, q, INT_VAR_RND(r), INT_VAL_SPLIT_MAX());
	}


	//constructor for cloning
	NQueens(bool share, NQueens& s, int size) : Script(share, s), n(size) {
		q.update(*this, share, s.q);
	}


	/// Perform copying during cloning
	virtual Space*
		copy(bool share) {
			return new NQueens(share, *this, n);
		}


	// print function to print Solution
	virtual void
		print(std::ostream& os) const {
			
			os << "Nqueensmatrix\t \t";
			os << std::endl;
			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < n; j++)
					
					os << q[i*n + j] << "," ;
			        os << std::endl;
			}
			os << std::endl;
		}

};

#if defined(GECODE_HAS_QT) && defined(GECODE_HAS_GIST)
/// Inspector showing queens on a chess board
class QueensInspector : public Gist::Inspector {
protected:
	/// The graphics scene displaying the board
	QGraphicsScene* scene;
	/// The window containing the graphics scene
	QMainWindow* mw;
	/// The size of a field on the board
	static const int unit = 20;
public:
	/// Constructor
	QueensInspector(void) : scene(NULL), mw(NULL) {}
	/// Inspect space \a s
	virtual void inspect(const Space& s) {
		const NQueens& q = static_cast<const NQueens&>(s);

		if (!scene)
			initialize();
		QList <QGraphicsItem*> itemList = scene->items();
		foreach(QGraphicsItem* i, scene->items()) {
			scene->removeItem(i);
			delete i;
		}

		for (int i = 0; i<q.q.size(); i++) {
			for (int j = 0; j<q.q.size(); j++) {
				scene->addRect(i*unit, j*unit, unit, unit);
			}
			QBrush b(q.q[i].assigned() ? Qt::black : Qt::red);
			QPen p(q.q[i].assigned() ? Qt::black : Qt::white);
			for (IntVarValues xv(q.q[i]); xv(); ++xv) {
				scene->addEllipse(QRectF(i*unit + unit / 4, xv.val()*unit + unit / 4,
					unit / 2, unit / 2), p, b);
			}
		}
		mw->show();
	}

	/// Set up main window
	void initialize(void) {
		mw = new QMainWindow();
		scene = new QGraphicsScene();
		QGraphicsView* view = new QGraphicsView(scene);
		view->setRenderHints(QPainter::Antialiasing);
		mw->setCentralWidget(view);
		mw->setAttribute(Qt::WA_QuitOnClose, false);
		mw->setAttribute(Qt::WA_DeleteOnClose, false);
		QAction* closeWindow = new QAction("Close window", mw);
		closeWindow->setShortcut(QKeySequence("Ctrl+W"));
		mw->connect(closeWindow, SIGNAL(triggered()),
			mw, SLOT(close()));
		mw->addAction(closeWindow);
	}

	/// Name of the inspector
	virtual std::string name(void) { return "Board"; }
	/// Finalize inspector
	virtual void finalize(void) {
		delete mw;
		mw = NULL;
	}
};

#endif /* GECODE_HAS_GIST */



// passing command line argumnets

int main(int argc, char* argv[]) {
	// commandline options
	SizeOptions opt("N");
	opt.solutions(1);
	opt.size(10);
	opt.parse(argc, argv);

	#if defined(GECODE_HAS_QT) && defined(GECODE_HAS_GIST)
		QueensInspector ki;
		opt.inspect.click(&ki);
	#endif
	// run script
	Script::run<NQueens, DFS, SizeOptions>(opt);
	return 0;
}
