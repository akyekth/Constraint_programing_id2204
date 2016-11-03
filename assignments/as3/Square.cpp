#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

int n;

class SquarePack : public Script {
public:

	//As per step1 : read s and x-coordinate and y cordinate values
	IntVar s;
	IntVarArray x;
	IntVarArray y;
	IntVarArray q;

	int max = 0;

	//As per step 1: create static method
	static int size(int i){
		return n - i;
	}

	SquarePack(const SizeOptions& opt)
		: Script(opt), x(*this, n, 0, floor(sqrt(n*(n + 1)*(2 * n + 1) / 6))), y(*this, n, 0, floor(sqrt(n*(n + 1)*(2 * n + 1) / 6))) {
		n = opt.size();
		max = (n*(n + 1)) / 2;
		s = IntVar(*this, floor(sqrt(n*(n + 1)*(2 * n + 1) / 6)), max);



		for (int i = 0; i < n - 1; i++) {
			rel(*this, (x[i] + size(i)) <= s);
			rel(*this, (y[i] + size(i)) <= s);
		}


		for (int i = 0; i<n - 1; i++) {
			for (int j = i + 1; j<n - 1; j++) {

				IntVar l(*this, 0, max);
				IntVar r(*this, 0, max);
				IntVar a(*this, 0, max);
				IntVar be(*this, 0, max);

				BoolVarArgs b(*this, 4, 0, 1);


				rel(*this, l == x[i] + size(i));
				rel(*this, r == x[j] + size(j));
				rel(*this, a == y[i] + size(i));
				rel(*this, be == y[j] + size(j));

				rel(*this, l, IRT_LQ, x[j], b[0]);
				rel(*this, r, IRT_LQ, x[i], b[1]);
				rel(*this, a, IRT_LQ, y[j], b[2]);
				rel(*this, be, IRT_LQ, y[i], b[3]);;

				linear(*this, b, IRT_GQ, 1);

			}
		}




		IntArgs siz = IntArgs(n);
		for (int i = 0; i < n; i++)
		{
			siz[i] = size(i);

		}

		for (int j = 0; j < s.max(); j++){
			BoolVarArgs b(*this, n, 0, 1);
			for (int i = 0; i < n - 1; i++){
				dom(*this, y[i], j,j + size(i), b[i]);
			}
			linear(*this, siz, b, IRT_LQ, s);
		}
		for (int j = 0; j < s.max(); ++j){
			BoolVarArgs b(*this, n, 0, 1);
			for (int i = 0; i < n - 1; i++){
				dom(*this, x[i],j, j + size(i), b[i]);
			}
			linear(*this, siz, b, IRT_LQ, s);
		}

		// as per step4 problem decomposition

		rel(*this, (s*s) >((n*(n + 1)*(2 * n + 1)) / 6));


		//step4 symmetry removal
		rel(*this, x[0] <= 1 + (s - n) / 2);
		rel(*this, x,IRT_GQ,y);


		//step4 Empty strip dominence 
		for (int i = 0; i < n - 1; ++i)
		{
			int q = size(i);
			int gap = 0;

			//step4 ignoring size 1 squares this is done by only looping till N-1(replace of N with N-1) 

			if (q == 2 || q == 4){ gap = 2; }
			else if (q == 3 || (q >= 5 && q <= 8)){ gap = 3; }
			else if (q <= 11) { gap = 4; }
			else if (q <= 17) { gap = 5; }
			else if (q <= 21) { gap = 6; }
			else if (q <= 29) { gap = 7; }
			else if (q <= 34) { gap = 8; }
			else if (q <= 44)  { gap = 9; }
			else if (q <= 45) { gap = 10; }

			if (gap != 0){

				rel(*this, x[i] != gap);
				rel(*this, y[i] != gap);
			}

		}

		// as per step5 Branching
		//a.always branch on s first
		//b.ist assign x coordinates then y coordinates
		//c.INT_VAR_NONE means it takes the biggest square as first in an array
		//d. INT_VAL_MIN means choose min value from L to R or R to L
		branch(*this, s, INT_VAL_MIN());
		branch(*this, x, INT_VAR_NONE(), INT_VAL_MIN());
		branch(*this, y, INT_VAR_NONE(), INT_VAL_MIN());
	}

	SquarePack(bool share, SquarePack& sq) : Script(share, sq) {
		x.update(*this, share, sq.x);
		y.update(*this, share, sq.y);
		s.update(*this, share, sq.s);
	}

	virtual Space*
		copy(bool share) {
			return new SquarePack(share, *this);
		}




	virtual void print(std::ostream& os) const {
		os << "\t";
		os << "The smallest square size = " << s << std::endl << "\t";

		for (int i = 0; i < n; i++) {
			if (i != n - 1) {
				os << "square " << size(i) << ":[" << x[i] << "," << y[i] << "]";
				os << std::endl << "\t";
			}
			else {
				os << "The smallest piece can be placed in any available location";
			}
		}
		os << std::endl;
	}

};

int main(int argc, char* argv[]) {
	SizeOptions opt("Square");
	int num;

	std::cout << "Enter N value" << std::endl;
	std::cin >> num;
	
	opt.size(num);
	n = num;
	opt.parse(argc, argv);
	Script::run<SquarePack, DFS, SizeOptions>(opt);
	return 0;
}