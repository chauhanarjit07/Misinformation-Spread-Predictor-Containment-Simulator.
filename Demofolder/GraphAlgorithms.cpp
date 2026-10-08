int getMaximumDegree(const Graph& graph){
    int maxDegree = 0;

    for (int i = 0; i < graph.getNumNodes(); i++){
        int degree = graph.getDegree(i);

        if (degree > maxDegree){
            maxDegree = degree;
        }
    }

    return maxDegree;
}