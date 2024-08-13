#include "pattern.h"
#include "malloc.h"

//--------------------------------------------------------------------------------
pattern::pattern(int size, int dim)
{
	//initialing structures
	m_coordinates = NULL;
	m_ddata = NULL;

	m_sizeList = size;
	m_dim = dim;
	m_colorModel = RGB;
}
//--------------------------------------------------------------------------------
pattern::pattern(const char *filename)
{
	//initialing structures
	m_coordinates = NULL;
	m_ddata = NULL;
	    
	m_sizeList = 0;
	m_dim = 0;
	    
	if(filename) loadfile(filename);
}
//--------------------------------------------------------------------------------
pattern::~pattern()
{
	//yes baby, kill it !
	if(m_coordinates) 	
		free(m_coordinates);
	
	if(m_ddata) 		
	{
 		for(unsigned int i=0; i<m_sizeList; i++)
			free(m_ddata[i].v);
		free(m_ddata);
	}
}
//--------------------------------------------------------------------------------
void pattern::setData(doubleVector *data)
{
	if(m_ddata) 		
	{
 		for(unsigned int i=0; i<m_sizeList; i++)
			free(m_ddata[i].v);
		free(m_ddata);
	}

	m_ddata = data;
}
//--------------------------------------------------------------------------------
void pattern::setCoordinates(point2D *coord)
{
    if(m_coordinates) 	
		free(m_coordinates);

    m_coordinates = coord;
}
//--------------------------------------------------------------------------------
bool pattern::loadfile(const char* filename)
{
	//openning a filename string
    FILE *arq;
    arq = fopen(filename, "r");
    printf("\n\tReading file '%s' ...", filename);
	
	if(!arq) {
		printf(" failed.\n");
		exit(1);
	}
    
    //just read the comments
    /*
    while ((c = getc(arq)) == '#')
    {
        while ((c = getc(arq)) != '\n');
        break;
    }
    */
    
    //reading number of dimensiosns
    m_dim = 3;
    
    //reading number of patterns
    fscanf(arq, "%d", &m_sizeList);
    printf("\n\tNumber of patterns: %d ...", m_sizeList);
    if(m_sizeList < 50)
    {
	  printf("\n Well, it is not an error, but is desired to use at least 50 samples to ensure a good statistical measure...\n");
	  printf("\n Try again and be a good boy !\n");
	  exit(1);
    }

    //if pattern is zero, kill-me !
    if(!m_sizeList) return false;  

	//more annoying comments
	/*
    c = getc(arq);
    c = getc(arq);
    while ((c = getc(arq)) == '#')
    {
        while ((c = getc(arq)) != '\n');
        break;
    }
    */
    
    //allocating memory
    m_coordinates 	= (point2D*) calloc(m_sizeList, sizeof(point2D));
    //m_colors		= (colorRGB*) calloc(m_sizeList, sizeof(colorRGB));
    m_ddata			= (doubleVector*) calloc(m_sizeList, sizeof(doubleVector));

    //read some more comments
    int x, y;

	for(unsigned int i=0; i<m_sizeList; i++)
    {
        //reading coordinates!
        fscanf(arq, "%d ", &x);
        fscanf(arq, "%d ", &y);     
		
		m_coordinates[i].x = x;
		m_coordinates[i].y = y;
        
        //some d-dimensional vector
		float v=0;
		m_ddata[i].v = (double*) calloc(m_dim, sizeof(double));
		for(unsigned int j=0; j<m_dim; j++)
		{
			fscanf(arq, "%f ",&v);
			m_ddata[i].v[j]=v;
		}
		//printf("\n\tReading pattern %d", i+1);
	}    
	//closing our filename
    fclose(arq);
    
	//if everything is fine, quit it !
    return true;
}
//--------------------------------------------------------------------------------
