/*Enter N value
9
Game of Life
The Solution =
0 0 | 0 0 0 0 0 0 0 0 0 | 0 0
0 0 | 0 0 0 0 0 0 0 0 0 | 0 0
---------------------------- -
0 0 | 1 1 0 1 1 0 0 1 1 | 0 0
0 0 | 1 1 0 1 0 1 0 0 1 | 0 0
0 0 | 0 0 0 1 0 0 1 1 0 | 0 0
0 0 | 1 1 0 1 1 0 1 0 0 | 0 0
0 0 | 1 1 0 1 0 0 1 0 0 | 0 0
0 0 | 0 0 0 1 0 0 1 1 0 | 0 0
0 0 | 1 1 0 1 1 0 1 0 0 | 0 0
0 0 | 1 1 0 1 0 0 1 0 0 | 0 0
0 0 | 0 0 0 0 1 1 0 0 0 | 0 0
---------------------------- -
0 0 | 0 0 0 0 0 0 0 0 0 | 0 0
0 0 | 0 0 0 0 0 0 0 0 0 | 0 0
Number of alive : 37

Initial
propagators : 754
  branchers : 1
   Summary
	  runtime : 0.120 (120.000 ms)
	  solutions : 1
	  propagations : 1890
	  nodes : 33
	  failures : 4
	  restarts : 0
	  no - goods : 0
	  peak depth : 26
*/


#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

int n;

class Life : public Script {
public:

	BoolVarArray q;
	IntVarArray optm;
	

	Life(const SizeOptions& opt) :Script(opt), q(*this, (opt.size() + 4)*(opt.size() + 4), 0, 1), optm(*this, pow(ceil(opt.size() / 3.0), 2), 0, 6) {
		//std::cout << "this is part1" << std::endl;
		// using 2 cells thickness so we are adding 4 to actual opt size
		int n1 = opt.size() + 4;
		
		// declaration of board size
		Matrix<BoolVarArgs> b(q, n1, n1);
		

		// set the values on border to 0
		for (int i = 0; i < 2; i++){

			linear(*this, b.row(i), IRT_EQ, 0);
			linear(*this, b.row(n1 - 1 - i), IRT_EQ, 0);
			linear(*this, b.col(i), IRT_EQ, 0);
			linear(*this, b.col(n1 - 1 - i), IRT_EQ, 0);

		}
		int index = 0;
		//loop inner row border outerborder is static(not change)
		for (int i = 1; i < n1-1; i++){
			for (int j = 1; j < n1 - 1; j++){
				
				//will take all neighbours in one array
				BoolVarArgs n2;
				

				// loop for neghbours of each cell like above,topleft,topright
				//left ,right,below,bottom left,bottom right
				for (int m = i - 1; m <= i + 1; m++){
					for (int k = j - 1; k <= j + 1; k++){
						if (!(m == i && k == j)){
							n2 << b(m, k);
													
						}
					}
				}
			

			
				// if the cell  is alive on the board sum of the neighbours should be 2 or 3
				//if the cell is dead on the board sum of the neighbours should not be 3
				rel(*this, (b(i, j) == 1 && sum(n2) >= 2 && sum(n2) <= 3) || (b(i, j) == 0 && sum(n2) != 3));
				

				// for every 3x3 square calculate sum of minisquares
				// for find those 3x3 squares
				
				BoolVarArgs s;
				
				if((i % 3 == 2) && (j % 3 == 2)&& i<n1-2 && j<n1-2){
										
						s << b(i,j) <<b(i,j+1) <<b(i,j+2)
							     <<b(i+1,j) << b(i+1,j+1) <<b(i+1,j+2)
							     << b(i+2,j) <<b(i+2,j+1) <<b(i+2,j+2);	

						rel(*this, optm[index] == sum(s) );
						
						index++;
					}
				

			}
			
		}

		
		// Branch over cels try hihg values first
		branch(*this,q , INT_VAR_NONE(), INT_VAL_MAX());

}
	
// constraint for finding better solutions
	virtual void constrain(const Space& _b) {

		const Life& b = static_cast<const Life&>(_b);
		rel(*this, sum(q) > sum(b.q));
		rel(*this, sum(optm) > sum(b.optm));
		
	}


	
// constructor for cloning
	Life(bool share, Life& l) : Script(share, l) {

		q.update(*this, share, l.q);		
	   optm.update(*this, share, l.optm);
		
	}

// perform copying during cloning
	virtual Space*
		copy(bool share) {
			return new Life(share, *this);
		}



   // print solution
	virtual void print(std::ostream& os) const {
		os << "\t";
		os << "The Solution = "  << std::endl ;
		int size = n + 4;
		int livecount=0;
		Matrix<BoolVarArray> cellmatrix(q, size, size);

		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++){
				if (j == 0 && (i == 2 || i == n + 2)){
					for (int k = 0; k < size; k++){
						std::cout << "--";
					}
					std::cout << "---" << std::endl; 
				}
				if (j == 2 || j == (n + 2)){
					std::cout << "|";
				}
				os << cellmatrix(i, j) << " ";
				livecount += cellmatrix(i, j).val();
			}
			
				os << std::endl  ;
		  }
			
		
		os << "Number of alive:" << livecount << std::endl ;
		os << std::endl ;
	}

};

// main method
int main(int argc, char* argv[]) {
	SizeOptions opt("Game of Life");
	int num;

	std::cout << "Enter N value" << std::endl;
	std::cin >> num;

	opt.size(num);
	opt.solutions(1);
	opt.iterations(1);
	n = num;
	opt.parse(argc, argv);
	Script::run<Life, BAB, SizeOptions>(opt);
	return 0;
}