#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <ctime>
#include <string>
#include <vector>
#include <utility>
#include "SimpleGraph.h"

using namespace std;

/* CONSTANTS */
const double kPi = 3.14159265358979323;
const double kRepel = 0.001;
const double kAttract = 0.001;

void Welcome();

/* This function reads a line from the standard input and returns it */
string GetLine(const string& prompt);

/*
 * This function prompts user for a file name and returns an
 * input file stream for that file
*/
ifstream GetInputFileStream();

/*
 * This function is used to parse a given input file stream
 * and return a Simple Graph from it
*/
void ParseGraphInputFileStream(ifstream& ifs, SimpleGraph& graph);

/* This function Reads a file and returns a Simple Graph from it */
SimpleGraph ReadSimpleGraph();

/* This function is used to initialize node positions */
void InitializeNodePositions(SimpleGraph& graph);

/*
 * This function is used to prompt and get the number of
 * seconds to run the algorithm from the user
*/
double GetSeconds();

/*
 * This function applies Force-Directed Graph Layout
 * for a given Graph
*/
void ForceDirectedLayout(SimpleGraph & graph);

/*
 * This function is used to apply Force-Directed Graph layout
 * algorithm for a given number of seconds
*/
void ApplyForceDirectedLayout(SimpleGraph& graph, double seconds);

// Main method
int main() {
    Welcome();
    /* Read a Graph from the Disk */
    SimpleGraph graph {ReadSimpleGraph()};

    /* Position the Nodes */
    InitializeNodePositions(graph);

    /* Initial Draw */
    DrawGraph(graph);

    /* Get the Number of Seconds */
    double seconds = GetSeconds();

    /* Apply Force-Directed Layout Algorithm  for the given number of seconds */
    ApplyForceDirectedLayout(graph, seconds);
    return 0;
}

/* Prints a message to the console welcoming the user and
 * describing the program. */
void Welcome() {
    cout << "Welcome to CS106L GraphViz!" << endl;
    cout << "This program uses a force-directed graph layout algorithm" << endl;
    cout << "to render sleek, snazzy pictures of various graphs." << endl;
    cout << endl;
}

/* My Definitions */
string GetLine(const string& prompt){
    string result {};
    while(true) {
        cout << prompt << flush;
        if(getline(cin , result)) return result;
    }
}

ifstream GetInputFileStream() {
    string fileName {};
    ifstream ifs {};

    while(true) {
        fileName = GetLine("Enter file name: ");
        if((ifs = ifstream{fileName})) return ifs;
        cout << "PLease enter a valid file name." << endl;
    }
}

void ParseGraphInputFileStream(ifstream& ifs, SimpleGraph& graph) {
  size_t numOfNodes {};

  if(ifs >> numOfNodes) {
    graph.nodes = vector<Node>(numOfNodes, {0.0, 0.0});
    size_t node1 {};
    size_t node2 {};
    while(true) {
      if( ifs >> node1 && ifs >> node2 ) {
        graph.edges.push_back({node1,node2});
      } else break;
    }

  }
}

SimpleGraph ReadSimpleGraph(){
    SimpleGraph graph {};
    ifstream graphIFS = GetInputFileStream();
    ParseGraphInputFileStream(graphIFS, graph);
    graphIFS.close();
    return graph;
}

void InitializeNodePositions(SimpleGraph& graph) {
    vector<Node> & nodes = graph.nodes;
    for(size_t k = 0; k < nodes.size(); ++k){
        Node & node = nodes.at(k);
        double value = (2 * kPi * k)/ nodes.size();
        node.x = cos(value);
        node.y = sin(value);
    }
}

double GetSeconds() {
    istringstream iss;
    double value {};
    while((iss = istringstream{GetLine("Enter the number of seconds: ")})) {
        if(iss >> value && value > 0.0) return value;
        cout << "Please, enter a valid number of seconds." << endl;
    }
    return value;
}

void ForceDirectedLayout(SimpleGraph & graph) {
    // initializations
    vector<Node> & nodes = graph.nodes; // set of nodes
    vector <Edge> & edges = graph.edges; // set of edges
    vector<pair<double, double>> netForces (nodes.size(), {0.0, 0.0}); // net force is the pair<Δx, Δy>

    // apply repulsive force
    for(size_t i = 0; i < nodes.size(); ++i){
        for(size_t j = i + 1; j < nodes.size(); ++j){
            // get references to nodes
            const double x0 = nodes[i].x;
            const double y0 = nodes[i].y;
            const double x1 = nodes[j].x;
            const double y1 = nodes[j].y;

            // get references to netforces
            double & dx0 = netForces[i].first;
            double & dy0 = netForces[i].second;
            double & dx1 = netForces[j].first;
            double & dy1 = netForces[j].second;

            // compute repulsive force
            double fRepel = kRepel / sqrt((pow((y1 - y0), 2.0) + pow((x1 - x0), 2.0)));
            double theta = atan2((y1 - y0), (x1 - x0));
            // update net forces
            dx0 -= fRepel * cos(theta);
            dy0 -= fRepel * sin(theta);
            dx1 += fRepel * cos(theta);
            dy1 += fRepel * sin(theta);
        }
    }

    // apply attractive force
    for (auto & edge : edges){
        // get references to nodes
        const double x0 = nodes[edge.start].x;
        const double y0 = nodes[edge.start].y;
        const double x1 = nodes[edge.end].x;
        const double y1 = nodes[edge.end].y;

        // get references to netforces
        double & dx0 = netForces[edge.start].first;
        double & dy0 = netForces[edge.start].second;
        double & dx1 = netForces[edge.end].first;
        double & dy1 = netForces[edge.end].second;

        // compute attractive forces
        double fAttract = kAttract * (pow((y1 - y0), 2.0) + pow((x1- x0), 2.0));
        double theta = atan2((y1 - y0), (x1 - x0));

        // update net forces
        dx0 += fAttract * cos(theta);
        dy0 += fAttract * sin(theta);
        dx1 -= fAttract * cos(theta);
        dy1 -= fAttract * sin(theta);
    }

    // change node position according to net force
    for (size_t k = 0; k < nodes.size(); ++k) {
        const double dx = netForces[k].first;
        const double dy = netForces[k].second;
        double & x = nodes[k].x;
        double & y = nodes[k].y;
        x += dx;
        y += dy;
    }

}

void ApplyForceDirectedLayout(SimpleGraph& graph, double seconds){
    double elapsedTime {0.0};
    time_t startTime = time(NULL);
    while(elapsedTime < seconds) {
        ForceDirectedLayout(graph);
        DrawGraph(graph);
        elapsedTime = difftime(time(NULL), startTime);
    }
}
