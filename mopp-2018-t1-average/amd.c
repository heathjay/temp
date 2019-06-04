#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//Linux
#include <pthread.h>
#include <assert.h>
#include <sys/sysinfo.h>
pthread_barrier_t barrier;


struct thread_work{
 //  uint32_t id;
   uint32_t start;
   uint32_t end;
   uint32_t v;
   uint32_t* dists;

   // uint32_t sum;
   // uint32_t path;
};
/*
    test using data
*/
int Loaddata (uint32_t* A, int n)
{

    FILE* ip;
        int i,j,temp;
        if ((ip=fopen("data_input","r"))==NULL)
        {
                printf("Error opening the input data.\n");
                return 1;
        }
        fscanf(ip,"%d\n\n",&temp);
    if (temp!=n)
    {
                printf("City count does not match the data!\n");
                return 2;
        }

        for (i=0;i<n;i++)
                for (j=0;j<n;j++)
                        fscanf(ip,"%d\t",A+n*i+j);
        fclose(ip);
        return 0;
    
}

int Savedata(uint32_t* A, int n)
{

    FILE* op;
        int i,j,temp;
        if ((op=fopen("data_output","w"))==NULL)
        {
                printf("Error opening the file.\n");
                return 1;
        }
        fprintf(op,"%d\n\n",n);
        for (i=0;i<n;i++)
    {
                for (j=0;j<n;j++)
                        fprintf(op,"%d\t",A[n*i+j]);
        fprintf(op,"\n");
    }
        fclose(op);
        
    return 0;
}
///////



// Solves the minimum distance between all pairs of vertices
void *md_all_pairs (void *tw) {
    struct thread_work *thread_work_d = (struct thread_work*)tw;
   // uint32_t id = thread_work_d  -> id;
    uint32_t start = thread_work_d  -> start;
    uint32_t end = thread_work_d  -> end;
    uint32_t v = thread_work_d  ->v;
    uint32_t* dists = thread_work_d  -> dists;
    uint32_t k, i, j;
    uint32_t infinity = v*v;


    for (k = 0; k < v; ++k) {
        int flag = pthread_barrier_wait(&barrier);
        if(flag != 0 && flag != PTHREAD_BARRIER_SERIAL_THREAD)
        {
            printf("Could not wait on barrier\n");
            exit(-1);
        }

        for (i = start; i < end; ++i) {
            uint32_t tempK = dists[i*v+k];
            if(k!= i && tempK!=UINT32_MAX){
                for (j = 0; j < v; ++j) {
                       if(i!=j && k!=j){
                        uint32_t tempJ = dists[k*v+j];
                        if (tempJ != UINT32_MAX){

                            uint32_t intermediary =tempK+ tempJ;
                        // Checks for overflows
                            if ((intermediary >= tempK)&&(intermediary >= tempJ)&&(intermediary < dists[i*v+j]))
                                dists[i*v+j] = intermediary;   
                        }
                    }             
                }
            }

        }
    }

}

/* Computes the average minimum distance between all pairs of vertices with a path connecting them */
void amd (uint32_t* dists, uint32_t v) {
    uint32_t i, j;
	uint32_t infinity = v*v;
    //uint32_t infinity = 1000;
	uint32_t smd = 0; 	//sum of minimum distances
	uint32_t paths = 0; //number of paths found
	uint32_t solution = 0;

    for (i = 0; i < v; ++i) {
        for (j = 0; j < v; ++j) {
			// We only consider if the vertices are different and there is a path
            if ((i != j) && (dists[i*v+j] < infinity)) {
				smd += dists[i*v+j];
				paths++;
			}
        }
    }

	solution = smd / paths;
	printf("%d\n",smd );
	printf("%d\n", solution);

}

/* Debug function (not to be used when measuring performance)*/
void debug (uint32_t* dists, uint32_t v) {
    uint32_t i, j;
	uint32_t infinity = v*v;

    for (i = 0; i < v; ++i) {
        for (j = 0; j < v; ++j) {
            if (dists[i*v+j] > infinity) printf("%7s", "inf");
            else printf ("%7u", dists[i*v+j]);
        }
        printf("\n");
    }
}

// Main program - reads input, calls FW, shows output
int main (int argc, char* argv[]) {

    //Reads input 
    //First line: v (number of vertices) and e (number of edges)
    uint32_t v, e;
    scanf("%u %u", &v, &e);
    //scanf("%u", &v);
    //Allocates distances matrix (w/ size v*v) i
    //and sets it with max distance and 0 for own vertex
    //20.05
    int cpus = get_nprocs();
    if (getenv("MAX_CPUS")){
        cpus = atoi(getenv("MAX_CPUS"));
    }
    assert(cpus > 0 && cpus <= 64);
    pthread_t thread[cpus];
    struct thread_work thread_work_d[cpus];
    if(pthread_barrier_init(&barrier,NULL, cpus+1)){
        printf("barrier initialization error\n");
        return -1;
    }

    uint32_t* dists = malloc(v*v*sizeof(uint32_t));
    memset(dists, UINT32_MAX, v*v*sizeof(uint32_t));
    uint32_t i;
    for ( i = 0; i < v; ++i ) dists[i*v+i] = 0;

    //Loaddata(dists,v);
    //Reads edges from file and sets them in the distance matrix
    uint32_t source, dest, cost;
    for ( i = 0; i < e; ++i ){
        scanf("%u %u %u", &source, &dest, &cost);
        if (cost < dists[source*v+dest]) dists[source*v+dest] = cost;
    }

//14.05
    //31.05
/*
    for(int i=0; i < cpus; i++){
         thread_work_d[i].id = i;
         thread_work_d[i].v = v;
         thread_work_d[i].start = i * v / cpus;
         thread_work_d[i].dists = dists;
         // thread_work_d[i].sum = 0;
         // thread_work_d[i].path = 0;
        if (i == cpus -1){     
            thread_work_d[i].end = v;    
        }else{
            thread_work_d[i].end = (i + 1) * v / cpus;
        }

        pthread_create(&thread[i],NULL,md_all_pairs,(void*)&thread_work_d[i]);
            
    }
*/

   for(int i=0; i < cpus; i++){
       //  thread_work_d[i].id = i;
         thread_work_d[i].v = v;
         thread_work_d[i].start = i * (v / cpus);
         thread_work_d[i].dists = dists;
         // thread_work_d[i].sum = 0;
         // thread_work_d[i].path = 0;
            thread_work_d[i].end = (i + 1) * (v / cpus);

        pthread_create(&thread[i],NULL,md_all_pairs,(void*)&thread_work_d[i]);
            
    }

 // main thread takes care of the leftover


    struct thread_work maintw;
    maintw.v  = v;
    maintw.start = (v/cpus) * cpus;
    maintw.end   = v;
    maintw.dists      = dists;
    //printf("bad\n" );
    //md_all_pairs((void*)&maintw);
   // printf("sys\n");

    md_all_pairs((void*)&maintw);

    // uint32_t sum = 0;
    // uint32_t path = 0;
 //pthread_create(&thread[i],NULL,md_all_pairs,(void*)&maintw);

    for (i = 0; i < cpus  ; i++){
        pthread_join (thread[i], NULL);

    }
    //printf("%d\n",*dists );
   // printf("%d\n",sum/path);
	//Computes the minimum distance for all pairs of vertices
    //md_all_pairs(dists, v);
    //Savedata(dists,v);
    //Computes and prints the final solution
    amd(dists, v);

#if DEBUG
	debug(dists, v);
#endif

    return 0;
}
