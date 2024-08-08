#include <bits/stdc++.h>
#include <chrono>
using namespace std;

map<string , vector<vector<string>>> cpt_map;

vector<vector<string>> 	data;
vector<int> 			missing_indices;
vector<string> 			var_data_order;

vector<vector<string>> 	bigdata;
vector<float> 		   	weights;

// Our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node{
private:
	string Node_Name;  // Variable name 
	string pos;
	vector<int> Children; // Children of a particular node - these are index of nodes in graph.
	vector<string> Parents; // Parents of a particular node- note these are names of parents
	vector<string> MarkovBlanket; // MarkovBlanket of a particular node- note these are names of markov blanket nodes
	int nvalues;  // Number of categories a variable represented by this node can take
	vector<string> values; // Categories of possible values
	vector<float> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning
public:
	// Constructor- a node is initialised with its name and its categories
    Graph_Node(string name,int n,vector<string> vals, string p){
		Node_Name=name;
		nvalues=n;
		values=vals;
		pos = p;
	}

	string get_name(){
		return Node_Name;
	}

	string get_pos(){
		return pos;
	}
	
	vector<int> get_children(){
		return Children;
	}

	vector<string> get_Parents(){
		return Parents;
	}

	vector<string> get_MarkovBlanket(){
		return MarkovBlanket;
	}

	vector<float> get_CPT(){
		return CPT;
	}

	int get_nvalues(){
		return nvalues;
	}

	vector<string> get_values(){
		return values;
	}

	void set_CPT(vector<float> new_CPT){
		CPT.clear();
		CPT=new_CPT;
	}

    void set_Parents(vector<string> Parent_Nodes){
        Parents.clear();
        Parents=Parent_Nodes;
    }

    void set_MarkovBlanket(vector<string> MB_Nodes){
		MarkovBlanket = MB_Nodes;
	}

    // add another node in a graph as a child of this node
    int add_child(int new_child_index ){
        for(int i=0;i<Children.size();i++){
            if(Children[i]==new_child_index)
                return 0;
        }
        Children.push_back(new_child_index);
        return 1;
    }
};

 // The whole network represted as a list of nodes
class network{

	// list <Graph_Node> Pres_Graph;
public:
	list <Graph_Node> Pres_Graph;
	int addNode(Graph_Node node){
		Pres_Graph.push_back(node);
		return 0;
	}
    
	int netSize(){
		return Pres_Graph.size();
	}

    // get the index of node with a given name
    int get_index(string val_name){
        list<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++){
            if(listIt->get_name().compare(val_name)==0)
                return count;
            count++;
        }
        return -1;
    }
	// get the node at nth index

    list<Graph_Node>::iterator get_nth_node(int n){
       list<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++){
            if(count==n)
                return listIt;
            count++;
        }
        return listIt; 
    }

    //get the iterator of a node with a given name
    list<Graph_Node>::iterator search_node(string val_name){
        list<Graph_Node>::iterator listIt;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++){
            if(listIt->get_name().compare(val_name)==0)
                return listIt;
        }
            cout<<"node not found\n";
        return listIt;
    }	
};

network Alarm;

network read_network(string file){
	network Alarm;
	string line;
	int find=0;
  	ifstream myfile(file); 
  	string temp;
  	string name;
  	vector<string> values;
  	
    if (myfile.is_open()){
    	while (! myfile.eof() ){
    		stringstream ss;
      		getline (myfile,line);
      		
      		ss.str(line);
     		ss>>temp;
     		
     		if(temp.compare("variable")==0){
                    
     			ss>>name;
     			getline (myfile,line);
                   
     			stringstream ss2;
     			ss2.str(line);
     			for(int i=0;i<4;i++){
     				ss2>>temp;
     			}
     			values.clear();
     			while(temp.compare("};")!=0){
     				values.push_back(temp);
     				ss2>>temp;
    			}
    			getline (myfile,line);

     			Graph_Node new_node(name,values.size(),values,line);
     			int pos=Alarm.addNode(new_node);     				
     		}
     		else if(temp.compare("probability")==0){
     			ss>>temp;
     			ss>>temp;
                list<Graph_Node>::iterator listIt;
                list<Graph_Node>::iterator listIt1;
     			listIt=Alarm.search_node(temp);
                int index=Alarm.get_index(temp);
                ss>>temp;
                values.clear();
     			while(temp.compare(")")!=0){
                    listIt1=Alarm.search_node(temp);
                    listIt1->add_child(index);
     				values.push_back(temp);
     				ss>>temp;
    			}
                listIt->set_Parents(values);
    			getline (myfile,line);
     			stringstream ss2;
                   
     			ss2.str(line);
     			ss2>> temp;
                   
     			ss2>> temp;
                    
     			vector<float> curr_CPT;
                string::size_type sz;
     			while(temp.compare(";")!=0){
     				curr_CPT.push_back(atof(temp.c_str()));
     				ss2>>temp;
    			}
                    
                listIt->set_CPT(curr_CPT);
     		}
            else{
                
            }    		
    	}
    	
    	if(find==1)
    	myfile.close();
  	}
  	
  	return Alarm;
}

void print_network(network net){
	cout << "network size : " + to_string(net.netSize()) << endl;

	for(int i = 0 ; i < net.netSize() ; i++){
		cout << "Variable Name - " ;
		cout << (*net.get_nth_node(i)).get_name() << endl;
		cout << "Possible Vals - ";
		vector<string> q = (*net.get_nth_node(i)).get_values();
		for (int i = 0; i < q.size(); ++i){
			cout << q[i] << ' ';
		}
		cout << endl;
		cout << "Dependencies  - " ;
		vector<string> v = (*net.get_nth_node(i)).get_Parents();
		for(auto itr = v.begin() ; itr != v.end(); itr++){
			cout << *itr << ' ';
		}
		cout << endl;
		cout << "CPT           - " ;
		vector<float> v1 = (*net.get_nth_node(i)).get_CPT();
		for (int i = 0; i < v1.size(); ++i){
			cout << v1[i] << ' ';
		}
		cout << endl;
	}
}

float roundy(float var) { 
    float value = (int)(var * 10000 + .5); 
    return (float)value / 10000; 
} 

void output_network(string out){
	ofstream fout;
    fout.open(out);

	fout << "network \"Alarm\" { "<< endl << "}" << endl;

	for(auto itr = Alarm.Pres_Graph.begin() ; itr != Alarm.Pres_Graph.end() ; itr++){
		fout << "variable  " << (*itr).get_name() << " {" << endl;
		fout << "    type discrete[" << (*itr).get_nvalues() << "] {  " ;
		vector<string> t = (*itr).get_values();
		for(int i = 0 ; i < t.size() ; i++){
			fout << t[i] << "  " ;
		}
		fout << "};" << endl;
		fout << (*itr).get_pos() << endl;
		fout << "}"  << endl;
	}

	for(auto itr = Alarm.Pres_Graph.begin() ; itr != Alarm.Pres_Graph.end() ; itr++){
		fout << "probability (  " << (*itr).get_name() << "  ";
		vector<string> t = (*itr).get_Parents();
		for(int i = 0 ; i < t.size() ; i++){
			fout << t[i] << "  ";
		}
		fout << ") {" << endl;
		fout << "    table ";
		vector<float> t1 = (*itr).get_CPT();
		for(int i = 0 ; i < t1.size() ; i++){
			fout << roundy(t1[i]) << " ";
		}
		fout << ";" << endl;
		fout << "}" << endl;
	}
	fout.close();
}

map<string , vector<vector<string>> > gen_cpt_map(){

	map<string , vector<vector<string>>> ret;
	list<Graph_Node> temp = Alarm.Pres_Graph;
	
	for(auto itr = temp.begin() ; itr != temp.end() ; itr++){
		vector<string> temp2 = (*itr).get_Parents();
		vector<vector<string>> temp3;
		temp3.push_back((*itr).get_values());
		for (int i = 0; i < temp2.size(); ++i){
			vector<string> vals = (*Alarm.search_node(temp2[i])).get_values();
			temp3.push_back(vals);
		}
		
		vector<vector<string>> qu;
		qu.push_back({""});
		while(temp3.size() != 0){
			vector<string> temp4 = temp3[temp3.size() - 1];
			int temp5 = qu.size();
			for(int i = 0 ; i < temp4.size() ; i++){
				for (int j = 0; j < temp5; ++j){
					vector<string> temp6 = qu[j];
					temp6.insert(temp6.begin() , temp4[i]);
					qu.push_back(temp6);				
				}
			}
			for(int i = 0 ; i < temp5 ; i++){
				qu.erase(qu.begin());
			}
			temp3.erase(temp3.end() - 1);
		}
		for(int i = 0 ; i < qu.size() ; i++){
			qu[i].erase(qu[i].end() - 1);
		}
		ret[(*itr).get_name()] = qu;
	}
	return ret;
}

void generate_markov_blanket(){

	network ret;
	for (auto itr = Alarm.Pres_Graph.begin() ; itr != Alarm.Pres_Graph.end() ; itr++){
		string self = (*itr).get_name();
  		vector<string> par = (*itr).get_Parents();
   		vector<int> c = (*itr).get_children();
   		std::vector<string> chi;

   		for(int i = 0 ; i < c.size() ; i++){
   			Graph_Node ty = *(Alarm.get_nth_node(c[i])); 
   			chi.push_back(ty.get_name());
   		}

   		std::vector<string> spo;
   		for(int i = 0 ; i < chi.size() ; i++){
   			Graph_Node t = *(Alarm.search_node(chi[i]));
   			vector<string> chipar = t.get_Parents();
   			spo.insert(spo.end() , chipar.begin() , chipar.end() );
	   	}
	   	set<string> temp;
	   	for(int i = 0 ; i < par.size() ; i++){
	   		temp.insert(par[i]);
	   	}
	   	for(int i = 0 ; i < chi.size() ; i++){
	   		temp.insert(chi[i]);
	   	}
	   	for(int i = 0 ; i < spo.size() ; i++){
	   		temp.insert(spo[i]);
	   	}
	   	std::vector<string> final;
	   	for(auto itr2 = temp.begin() ; itr2 != temp.end() ; itr2++){
	   		final.push_back(*itr2);
	   	}
	   	// int st;
	   	for(int i = 0 ; i < final.size() ; i++){
	   		if(final[i] == self){
	   			final.erase(final.begin() + i);
	   			break;
	   		}
	   	}
	   	Graph_Node gn = *itr;
	   	gn.set_MarkovBlanket(final);
	   	ret.addNode(gn);


    }
    Alarm = ret;
}

void make_missing_index(){

	for(int i = 0 ; i < data.size() ; i++){
		for(int j = 0 ; j < data[i].size() ; j++){
			if(data[i][j] == "\"?\""){
				missing_indices.push_back(j);
				break;
			}
			if( j == (data[i].size() - 1) ){
				missing_indices.push_back(-1);
				break;
			}
		}
	}
}

void load_data(string datafile){

	ifstream myfile(datafile); 
  	string temp;
  	string name;
  	string line;
  	vector<string> values;
  	
    if (myfile.is_open()){

    	while (! myfile.eof() ){
    		stringstream ss;
      		getline (myfile,line);
      		
      		values.clear();
      		ss.str(line);
      		for(int i = 0 ; i < 37 ; i++){
      			ss>>temp;
      			values.push_back(temp);
      			// values.push_back(temp.substr(1,temp.size() - 2));	
      		}
      		data.push_back(values);
     		
    	}
    	myfile.close();
  	}
}

float get_prob(vector<string> var_data_order , vector<string> var_names , vector<string> cpt_entries){

	float num = 0;
	float den = 0;
	vector<int> indices;
	for(int i = 0 ; i < var_names.size() ; i++){
		for(int j = 0 ; j < var_data_order.size() ; j++){
			if(var_names[i] == var_data_order[j]){
				indices.push_back(j);
				break;
			}
		}	
	}

	for(int i = 0 ; i < data.size() ; i++){
		bool b = true;
		for(int j = 1 ; j < indices.size() ; j++){
			b = b && ( data[i][indices[j]] == cpt_entries[j] );
		}
		if(b){
			den++;
			if(data[i][indices[0]] == "\"?\"")
				den--;
			if(data[i][indices[0]] == cpt_entries[0])
				num++;
			
		}
	}
	if(den == 0)	{
		den++;}
	float ans = num/den;
	if(ans == 0)
		return 0.0001;
	else
		return ans;
}

void init_params(map<string , vector<vector<string>> > cpt_map){
	network net;
	list<Graph_Node> temp = Alarm.Pres_Graph;

	

	for (auto itr = temp.begin() ; itr != temp.end() ; itr++){
		vector<string> t1 = (*itr).get_Parents();

		string var = (*itr).get_name();
		t1.insert(t1.begin() , var);
		vector<vector<string>> cpt_entries = cpt_map[var];

		vector<float> newCPT;
		for(int i = 0 ; i < cpt_entries.size() ; i++){
			float p = get_prob(var_data_order , t1 , cpt_entries[i]);
			newCPT.push_back(p);
		}

		Graph_Node t = *itr;
		(t).set_CPT(newCPT);
		net.addNode(t);
		
	}
	Alarm = net;
}

void init_params2(map<string , vector<vector<string>> > cpt_map){
	network net;
	list<Graph_Node> temp = Alarm.Pres_Graph;

	srand(time(0)); 

	for (auto itr = temp.begin() ; itr != temp.end() ; itr++){
		vector<string> t1 = (*itr).get_Parents();

		string var = (*itr).get_name();
		t1.insert(t1.begin() , var);
		vector<vector<string>> cpt_entries = cpt_map[var];

		vector<float> newCPT;
		for(int i = 0 ; i < cpt_entries.size() ; i++){
			float p = ((float) rand() / (RAND_MAX));
			newCPT.push_back(p);
		}

		Graph_Node t = *itr;
		(t).set_CPT(newCPT);
		net.addNode(t);
		
	}
	Alarm = net;
}

float get_weighted_prob(vector<string> var_names , vector<string> cpt_entries){

	float num = 0;
	float den = 0;
	vector<int> indices;
	for(int i = 0 ; i < var_names.size() ; i++){
		for(int j = 0 ; j < var_data_order.size() ; j++){
			if(var_names[i] == var_data_order[j]){
				indices.push_back(j);
				break;
			}
		}	
	}

	for(int i = 0 ; i < bigdata.size() ; i++){
		bool b = true;
		for(int j = 1 ; j < indices.size() ; j++){
			b = b && ( bigdata[i][indices[j]] == cpt_entries[j] );
		}
		float wt = weights[i];
		if(b){
			den = den + wt;
			if(bigdata[i][indices[0]] == cpt_entries[0])
				num = num + wt;
		}
	}
	if(den == 0)	{
		den++;//cout << "oye" << endl;
	}
	float ans = num/den;
	if(ans == 0)
		return 0.0001;
	else
		return ans;
}

network maximisation(){
	network net;
	list<Graph_Node> temp = Alarm.Pres_Graph;

	vector<string> var_data_order;
	for(auto itr = Alarm.Pres_Graph.begin() ; itr != Alarm.Pres_Graph.end() ; itr++){
		var_data_order.push_back((*itr).get_name());
	}

	for (auto itr = temp.begin() ; itr != temp.end() ; itr++){
		vector<string> t1 = (*itr).get_Parents();

		string var = (*itr).get_name();
		t1.insert(t1.begin() , var);
		vector<vector<string>> cpt_entries = cpt_map[var];

		vector<float> newCPT;
		for(int i = 0 ; i < cpt_entries.size() ; i++){
			float p = get_weighted_prob(t1 , cpt_entries[i]);
			newCPT.push_back(p);
		}

		Graph_Node t = *itr;
		t.set_CPT(newCPT);
		net.addNode(t);
		
	}
	return net;
}

float error(network net1 , network net2){

	float error = 0;
	for(int i = 0 ; i < net1.Pres_Graph.size() ; i++){
		Graph_Node g1 = *net1.get_nth_node(i);
		Graph_Node g2 = *net2.get_nth_node(i);
		vector<float> cpt1 = g1.get_CPT();
		vector<float> cpt2 = g2.get_CPT();
		for(int j = 0 ; j < cpt1.size() ; j++){
			error = error + abs(cpt1[j] - cpt2[j]);
		}
	}
	return error;
}

vector<float> normalize(vector<float> in){
	float tot = 0;
	for(int i = 0 ; i < in.size() ; i++){
		tot = tot + in[i];
	}
	for(int i = 0 ; i < in.size() ; i++){
		in[i] = in[i]/tot;
	}
	return in;
}

map<string , float> markovblanketsampling(vector<string> datarow , int miss){
	map<string , float> ret;

	string varX      = var_data_order[miss];
	Graph_Node gn    = *Alarm.search_node(varX);
	vector<int> temp = (gn).get_children();
	vector<Graph_Node> varXc;
	for (int i = 0; i < temp.size(); ++i){
		varXc.push_back( *Alarm.get_nth_node(temp[i]) );
	}
	varXc.insert(varXc.begin() , gn);

	vector<string> poss_val = gn.get_values();

	vector<float> ert;
	for(int q = 0 ; q < poss_val.size() ; q++){
		string poss = poss_val[q];

		float common = 1;
		for(int i = 0 ; i < varXc.size() ; i++){
			string temp1 = varXc[i].get_name();
			vector<string> temp2 = varXc[i].get_Parents();
			temp2.insert(temp2.begin() , temp1);
			vector<int> temp3;
			for(int j = 0 ; j < temp2.size() ; j++){
				for(int k = 0 ; k < var_data_order.size() ; k++){
					if(temp2[j] == var_data_order[k]){
						temp3.push_back(k);
						break;
					}
				}
			}
			vector<string> temp4;
			for(int j = 0 ; j < temp3.size() ; j++){
				if(datarow[temp3[j]] == "\"?\"")
					temp4.push_back(poss);
				else
					temp4.push_back(datarow[temp3[j]]);
			}
			vector<vector<string>> temp5 = cpt_map[temp1];
			int gotit = -1;
			for(int j = 0 ; j < temp5.size() ; j++){
				if(temp5[j] == temp4){
					gotit = j; 
					break;
				}
			}

			vector<float> temp6 = varXc[i].get_CPT(); 
			common = common * temp6[gotit];
		}
		ert.push_back(common);
	}
	ert = normalize(ert);

	for(int i = 0 ; i < poss_val.size() ; i++){
		ret[poss_val[i]] = ert[i];
	}
	// ret[poss] = common;
	return ret;
}

void printv( vector<string> in){
	for(int i = 0 ; i < in.size() ; i++){
		cout << in[i] << ' ';
	}
	cout << endl;
}

void expectation(){

	bigdata.clear();
	weights.clear();

	for(int i = 0 ; i < data.size() ; i++){

		if(missing_indices[i] >= 0){
			vector<string> temp3 = data[i];
			map<string , float> temp2 = markovblanketsampling(temp3 ,missing_indices[i]);
			// cout << "check4" << endl;
			for( auto itr : temp2){
				vector<string> temp4;
				for(int j = 0 ; j < temp3.size() ; j++){
					if(temp3[j] == "\"?\"")
						temp4.push_back(itr.first);
					else
						temp4.push_back(temp3[j]);
				}

				bigdata.push_back(temp4);
				// printv(temp4);
				weights.push_back(itr.second);
				// cout << itr.second << endl;
			}
		}
		else{
			cout << "a" << endl;
			bigdata.push_back(data[i]);
			weights.push_back(1);
			continue;
		}
	}
}

int main(int argc, char const *argv[]){

	chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	chrono::steady_clock::time_point end;

	Alarm = read_network(argv[1]);
	// network GoldAlarm = read_network("gold_alarm.bif");

	for(auto itr = Alarm.Pres_Graph.begin() ; itr != Alarm.Pres_Graph.end() ; itr++){
		var_data_order.push_back((*itr).get_name());
	}

	generate_markov_blanket();
	
	load_data(argv[2]);
	make_missing_index();

	cpt_map = gen_cpt_map();
	
	init_params2(cpt_map);

	network new_net;
	int timelimit = 117; // seconds
	int i = 0;
	int po = 2;
	// cout << "Initial Error : " << error(Alarm , GoldAlarm) << endl;
	while(true){
		
		expectation();

		new_net = maximisation();
		
		end = std::chrono::steady_clock::now();
		int timediff = chrono::duration_cast<chrono::microseconds>(end - begin).count();
		if(timediff > timelimit*1000000){   //error(new_net , Alarm) < 0.00001 || 
			Alarm = new_net;
			break;
		}

		Alarm = new_net;
		// cout << "Iteration " << i+1 << " complete" << endl;
		// cout << "Error : " << error(Alarm , GoldAlarm) << endl;
		i++;

	}

	output_network("solved_alarm.bif");

}