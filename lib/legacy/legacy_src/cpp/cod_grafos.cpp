#include <bits/stdc++.h>
#define rep(i, a, b) for(int i = a; i < (b); ++i)
#define all(x) begin(x), end(x)
#define sz(x) (int)(x).size()

using namespace std;
using vi = vector<int>;
typedef vector<vector<bool>> vb;
const int MAX_VERTEX = 12;
const int MAX_EDGES = MAX_VERTEX * (MAX_VERTEX - 1) / 2;

void save_truth_table(int num_vars, const vector<bool>& f) {
    int e = num_vars;
    int t = f.size();
    
    std::ofstream table_file("table.csv");
    
    // Línea de encabezados del csv
    for (int i = 0; i < e; i++)
        table_file << "arista_" << i+1 << ",";
    table_file << "f";
    table_file << "\n";

    // Filas de la tabla de verdad del csv
    for (int g = 0; g < t; g++) {
        // Mintérmino
        bitset<MAX_EDGES> setting = bitset<MAX_EDGES>(g);
        for (int i = 0; i < e; i++)
            table_file << setting[i] << ",";
        // Valor de la función f
        table_file << f[g];
        table_file << "\n";
    }
    table_file << "\n";

    table_file.close();
}

void save_sequence(const vector<bool>& f) {
    std::ofstream sequence_file("sequence.txt");

    for (bool b : f)
        sequence_file << b;

    sequence_file.close();
}


struct Maxclique {
	double limit=0.025, pk=0;
	struct Vertex { int i, d=0; };
	typedef vector<Vertex> vv;
	vb e;
	vv V;
	vector<vi> C;
	vi qmax, q, S, old;
	void init(vv& r) {
		for (auto& v : r) v.d = 0;
		for (auto& v : r) for (auto j : r) v.d += e[v.i][j.i];
		sort(all(r), [](auto a, auto b) { return a.d > b.d; });
		int mxD = r[0].d;
		rep(i,0,sz(r)) r[i].d = min(i, mxD) + 1;
	}
	void expand(vv& R, int lev = 1) {
		S[lev] += S[lev - 1] - old[lev];
		old[lev] = S[lev - 1];
		while (sz(R)) {
			if (sz(q) + R.back().d <= sz(qmax)) return;
			q.push_back(R.back().i);
			vv T;
			for(auto v:R) if (e[R.back().i][v.i]) T.push_back({v.i});
			if (sz(T)) {
				if (S[lev]++ / ++pk < limit) init(T);
				int j = 0, mxk = 1, mnk = max(sz(qmax) - sz(q) + 1, 1);
				C[1].clear(), C[2].clear();
				for (auto v : T) {
					int k = 1;
					auto f = [&](int i) { return e[v.i][i]; };
					while (any_of(all(C[k]), f)) k++;
					if (k > mxk) mxk = k, C[mxk + 1].clear();
					if (k < mnk) T[j++].i = v.i;
					C[k].push_back(v.i);
				}
				if (j > 0) T[j - 1].d = 0;
				rep(k,mnk,mxk + 1) for (int i : C[k])
					T[j].i = i, T[j++].d = k;
				expand(T, lev + 1);
			} else if (sz(q) > sz(qmax)) qmax = q;
			q.pop_back(), R.pop_back();
		}
	}
	vi maxClique() { init(V), expand(V); return qmax; }
	Maxclique(vb conn) : e(conn), C(sz(e)+1), S(sz(C)), old(S) {
		rep(i,0,sz(e)) V.push_back({i});
	}
};

vector<vector<bool>> decode_graph(const vector<bool>& code_edges, int v, int e) {
    vector<vector<bool>> graph_matrix(v, vector<bool>(v, false));

    int k = 0;
    for (int i = 1; i < v; i++) {
        for (int j = 0; j < i; j++) {
            graph_matrix[i][j] = code_edges[k];
            graph_matrix[j][i] = code_edges[k];
            k++;
        }
    }

    return graph_matrix;
}

void generate_f_clique_incrementally() {
    int e = 1;
    int t = 2;
    int pot_2 = 2;
    for (int v = 2; v <= MAX_VERTEX; v++) {
        pot_2 *= 2;
        vector<bool> f(t, false);
        
        // Generar las "t" cadenas de tamaño "e"
        for (int g = 0; g < t; g++) {
            // Para cada cadena (codificación de grafo), calcular el mayor cliqué y ver si es mayor que n/2
            bitset<MAX_EDGES> setting = bitset<MAX_EDGES>(g);
            vector<bool> vec_setting(e);
            for (int i = 0; i < vec_setting.size(); i++)
                vec_setting[i] = setting[i];
            vector<vector<bool>> matrix = decode_graph(vec_setting, v, e);
            Maxclique clique_calculator = Maxclique(matrix);
            vi max_clique = clique_calculator.maxClique();
            f[g] = (max_clique.size() >= (v / 2));
        }
        // DEBUG
        cout << "Grafos de tamaño " << v << ":\n";
        for (int i = 0; i < f.size(); i++) {
            if (i % 10 == 0)
                cout << "\n\tFila " << i / 10 << " -> ";
            cout << f[i];
        }
        cout << "\n";
        
        e = e + v;
        t = pot_2 * t;
    }

    return;
}

void generate_f_clique_n(int n) {
    int v = n;
    int e = v * (v-1) / 2;
    int t = 1;
    for (int i = 0; i < e; i++)
        t *= 2;

    vector<bool> f(t, false);        
    // Generar las "t" cadenas de tamaño "e"
    for (int g = 0; g < t; g++) {
        // Para cada cadena (codificación de grafo), calcular el mayor cliqué y ver si es mayor que n/2
        bitset<MAX_EDGES> setting = bitset<MAX_EDGES>(g);
        vector<bool> vec_setting(e);
        for (int i = 0; i < vec_setting.size(); i++)
            vec_setting[i] = setting[i];
        vector<vector<bool>> matrix = decode_graph(vec_setting, v, e);
        Maxclique clique_calculator = Maxclique(matrix);
        vi max_clique = clique_calculator.maxClique();
        f[g] = (max_clique.size() >= (v / 2));
    }

    save_sequence(f);
    return;
}

void generate_parity(int n) {
    int v = n;
    int e = v * (v-1) / 2;
    int t = 1;
    for (int i = 0; i < e; i++)
        t *= 2;

    vector<bool> f(t, false);        
    // Generar las "t" cadenas de tamaño "e"
    for (int g = 0; g < t; g++) {
        // Para cada cadena (codificación de grafo), calcular el mayor cliqué y ver si es mayor que n/2
        bitset<MAX_EDGES> setting = bitset<MAX_EDGES>(g);
        int parity = 0;
        for (int i = 0; i < e; i++)
            parity = (parity + setting[i]) % 2;

        f[g] = parity == 0;
    }

    save_sequence(f);
    return;
}

void save_graph_and_clique(int graph_number, int v) {
    int e = v * (v-1) / 2;
    bitset<MAX_EDGES> setting = bitset<MAX_EDGES>(graph_number);
    vector<bool> vec_setting(e);
    for (size_t i = 0; i < vec_setting.size(); i++)
        vec_setting[i] = setting[i];
    vector<vector<bool>> matrix = decode_graph(vec_setting, v, e);
    Maxclique clique_calculator = Maxclique(matrix);
    vi max_clique = clique_calculator.maxClique();
    
    std::ofstream graph_file("graph.txt");
    graph_file << "[\n";
    for (int i = 0; i < matrix.size(); i++) {
        graph_file << "\t[";
        if (matrix[i].size() > 0)
            graph_file << matrix[i][0];
        for (int j = 1; j < matrix[i].size(); j++)
            graph_file << ", " << matrix[i][j];
        graph_file << "],\n";
    }
    graph_file << "]";

    std::ofstream clique_file("clique.txt");
    clique_file << "[\n";
    clique_file << "\t";
    clique_file << max_clique[0];
    for (int j = 1; j < max_clique.size(); j++)
        clique_file << ", " << max_clique[j];
    clique_file << "\n]";
    
    clique_file.close();
}

int main() {

    // generate_graphs_incrementally();
    generate_parity(6);
    // save_graph_and_clique(2304 * 10 + 4, 6);

    return 0;
}