#include <coap.h>
#include <stdio.h>
#define TESTMODE
#define TEMPMODE

#ifdef __GNUC__
#define UNUSED_PARAM __attribute__ ((unused))
#else /* not a GCC */
#define UNUSED_PARAM
#endif /* GCC */

#ifdef TESTMODE
#include <stdlib.h>
#include <string.h>
#include <time.h>
/* self-referential structure */
struct topicData {            
	char* path;
	char * data;		/* each topicData contains a string */
    time_t topic_ma;	// max-age for topic in time after epoch
    time_t data_ma;		// max-age for data in time after epoch
    struct topicData *nextPtr; /* pointer to next node*/ 
}; /* end structure topicData */

typedef struct topicData TopicData; /* synonym for struct topicData */
typedef TopicData *TopicDataPtr; /* synonym for TopicData* */

/* prototypes */
TopicDataPtr getTopic( TopicDataPtr *sPtr, char* path);
int			setTopic( TopicDataPtr *sPtr, 
					char* path, char* data, size_t data_size, 
					time_t topic_ma, time_t data_ma);
					
int 		topicExist(TopicDataPtr *sPtr, char* path );				
int			addTopic(TopicDataPtr *sPtr,
					char* path, size_t path_size,
					time_t topic_ma);
int			addTopicWEC(TopicDataPtr *sPtr,
					char* path, size_t path_size,
					time_t topic_ma);
int 		deleteTopic( TopicDataPtr *sPtr, char* path );					
int			updateTopicInfo(TopicDataPtr *sPtr,
					char* path, time_t topic_ma);
					
int			updateTopicData(TopicDataPtr *sPtr,
					char* path, time_t data_ma,
					char* data, size_t data_size);


int 		DBEmpty( TopicDataPtr sPtr );
void 		printDB( TopicDataPtr currentPtr );

int 		compareString(char* a, char* b){
	if (a == NULL || b == NULL) return 0;
	if (strcmp(a,b) == 0)	return 1;
	else return 0;
}

/* start function get */
TopicDataPtr getTopic( TopicDataPtr *sPtr, char* path)
{ 
    TopicDataPtr previousPtr = NULL; /* pointer to previous node in list */
    TopicDataPtr currentPtr = NULL;  /* pointer to current node in list */
    TopicDataPtr tempPtr = NULL;     /* temporary node pointer */
	
	if (path == NULL || DBEmpty(*sPtr)) { return NULL;} 
	
    /* delete first node */
  
    if ( compareString(( *sPtr )->path,path)) { 
        tempPtr = *sPtr; /* hold onto node being removed */
        return tempPtr;
    } /* end if */
    else { 
        previousPtr = *sPtr;
        currentPtr = ( *sPtr )->nextPtr;

        /* loop to find the correct location in the list */
        while ( currentPtr != NULL && !compareString(currentPtr->path,path) ) { 
            previousPtr = currentPtr;         /* walk to ...   */
            currentPtr = currentPtr->nextPtr; /* ... next node */  
        } /* end while */

        /* delete node at currentPtr */
        if ( currentPtr != NULL ) { 
            tempPtr = currentPtr;
            return tempPtr;
        } /* end if */
     
    } /* end else */

    return NULL;

} 

int setTopic( TopicDataPtr *sPtr, 
					char* path, char* data, size_t data_size, 
					time_t topic_ma, time_t data_ma)
{ 
	
	TopicDataPtr topic = getTopic(sPtr, path);
	if (topic != NULL){
		if( data_size > 0 && data != NULL){
			char* temp_data = malloc(sizeof(char) * (data_size + 2));
			snprintf(temp_data,sizeof(char) * (data_size + 1), "%s", data);
			
			if (topic->data != NULL){
				free( topic->data );
			}
			topic->data			= temp_data;
		}
		else {
			topic->data			= NULL;
		}
		
		topic->topic_ma		= topic_ma;
		topic->data_ma 		= data_ma;
		return 1;
	}
	
    return 0;

} 

int topicExist(TopicDataPtr *sPtr, char* path )
{ 
    TopicDataPtr topic = getTopic(sPtr, path);
	if (topic == NULL) return 0;
	else return 1;

} 

int	addTopic(TopicDataPtr *sPtr,
					char* path, size_t path_size,
					time_t topic_ma){
					
    TopicDataPtr newPtr = NULL;      /* pointer to new node */
    TopicDataPtr previousPtr = NULL; /* pointer to previous node in list */
    TopicDataPtr currentPtr = NULL;  /* pointer to current node in list */

    newPtr = malloc( sizeof( TopicData ) ); /* create node on heap */
	
    if ( newPtr != NULL ) { /* is space available */
		
		
		/* add data to new struct here */
		if (path_size > 0 && path != NULL){
			char* temp_path = malloc(sizeof(char) * (path_size + 2));		
			snprintf(temp_path,sizeof(char) * (path_size+1), "%s", path);
			
			newPtr->path = temp_path; /* place value in node */
			newPtr->data = NULL; /* place value in node */
			newPtr->data_ma = 0; /* place value in node */
			newPtr->topic_ma = topic_ma; /* place value in node */
			newPtr->nextPtr = NULL; /* node does not link to another node */
			/* add data to new struct here */
		}
		else {
			free(newPtr);
			return 0;
		}
		
        previousPtr = NULL;
        currentPtr = *sPtr;

        /* loop to find the correct location in the list */
        while ( currentPtr != NULL) { 
			//TODO: if data available, stop. update topic_ma and ct
			
            previousPtr = currentPtr;          /* walk to ...   */
            currentPtr = currentPtr->nextPtr;  /* ... next node */
        } /* end while */

        /* insert new node at beginning of list */
        if ( previousPtr == NULL ) { 
            newPtr->nextPtr = *sPtr;
            *sPtr = newPtr;
        } /* end if */
        else { /* insert new node between previousPtr and currentPtr */
            previousPtr->nextPtr = newPtr;
            newPtr->nextPtr = currentPtr;
        } /* end else */
		return 1;
    } /* end if */
    else {
        printf( "%s not inserted. No memory available.\n", path );
		return 0;
    } /* end else */

} 

int	addTopicWEC(TopicDataPtr *sPtr,
					char* path, size_t path_size,
					time_t topic_ma){
	
	TopicDataPtr temp = getTopic(sPtr, path);
	if(temp != NULL){
		
		return updateTopicInfo(sPtr,path, topic_ma);
	}
	else {
		return addTopic( sPtr, path, path_size, topic_ma);
	}
					
}
int deleteTopic( TopicDataPtr *sPtr, char* path)
{ 
    TopicDataPtr previousPtr = NULL; /* pointer to previous node in list */
    TopicDataPtr currentPtr = NULL;  /* pointer to current node in list */
    TopicDataPtr tempPtr = NULL;     /* temporary node pointer */

	if (path == NULL || DBEmpty(*sPtr)) { return 0;}
	
    /* delete first node */
    if ( compareString(( *sPtr )->path,path)) { 
        tempPtr = *sPtr; /* hold onto node being removed */
        *sPtr = ( *sPtr )->nextPtr; /* de-thread the node */
        free( tempPtr->data );
        free( tempPtr->path );
        free( tempPtr ); /* free the de-threaded node */
        return 1;
    } /* end if */
    else { 
        previousPtr = *sPtr;
        currentPtr = ( *sPtr )->nextPtr;

        /* loop to find the correct location in the list */
        while ( currentPtr != NULL && !compareString(currentPtr->path,path) ) { 
            previousPtr = currentPtr;         /* walk to ...   */
            currentPtr = currentPtr->nextPtr; /* ... next node */  
        } /* end while */

        /* delete node at currentPtr */
        if ( currentPtr != NULL ) { 
            tempPtr = currentPtr;
            previousPtr->nextPtr = currentPtr->nextPtr;
            free( tempPtr->data );
            free( tempPtr->path );
            free( tempPtr );
            return 1;
        } /* end if */
     
    } /* end else */

    return 0;

} /* end function delete */

int updateTopicInfo(TopicDataPtr *sPtr,
					char* path, time_t topic_ma){
						
	TopicDataPtr temp = getTopic(sPtr, path);
	if(temp != NULL){
		size_t data_size = 0;
		if (temp->data == NULL) {
			data_size = 0;
		}
		else {
			data_size = strlen(temp->data);
		}
		return setTopic(sPtr,path, temp->data, data_size, topic_ma, temp->data_ma);
	}
	else {
		return 0;
	}
	
} /* end function delete */


/* start function set */
int updateTopicData(TopicDataPtr *sPtr,
					char* path, time_t data_ma,
					char* data, size_t data_size){
   
	TopicDataPtr temp = getTopic(sPtr, path);
	if(temp != NULL){
		
		return setTopic(sPtr,path, data, data_size, temp->topic_ma, data_ma);
	}
	else {
		return 0;
	}

} /* end function delete */


/* Return 1 if the list is empty, 0 otherwise */
int DBEmpty( TopicDataPtr sPtr )
{ 
    return sPtr == NULL;

} /* end function isEmpty */

/* Print the list */
void printDB( TopicDataPtr currentPtr )
{ 

    /* if list is empty */
    if ( currentPtr == NULL ) {
        printf( "List is empty.\n\n" );
    } /* end if */
    else { 
        printf( "The list is:\n" );

        /* while not the end of the list */
        while ( currentPtr != NULL ) { 
            printf( "%s %s %d %d --> ",currentPtr->path, currentPtr->data,
            (int)currentPtr->topic_ma, (int)currentPtr->data_ma);
            currentPtr = currentPtr->nextPtr;   
        } /* end while */

        printf( "NULL\n\n" );
    } /* end else */

} /* end function printList */


/* Link Format Parser starts here */

int optionValidation(char* source){ 
  char * pch;
  int counter = 0;
  pch=strchr(source,'=');
  while (pch!=NULL)
  {
    counter++;
    pch=strchr(pch+1,'=');
  }
	return counter;
}

int calOptionSize(char* source, int* type, int* data){
	
	if (optionValidation(source) == 1){
		*type = (int) strcspn(source, "=");
		char* pch = strchr(source,'=')+1;
		*data = (int) strlen(pch);
		return 1;			
	}
	return 0;
} 

int parseOption(char * source, char* type, char* data){
 
		if (optionValidation(source) == 1){
			
			int counter = strcspn(source, "=");
			char* pch = strchr(source,'=');
			if (pch != NULL){
				strncpy(type,source,counter);
				type[counter] = '\0';
				
				if(pch[1] == '"')
					strcpy(data,pch+2);
				else
					strcpy(data,pch+1);
					
				if(pch[strlen(pch+1)] == '"')
					data[strlen(data)-1] = '\0';
					
				return 1;
			}
		}
		return 0;
}
 
int calPathSize(char* source){
	
	if (source[0] == '<' && source[1] != '/' &&source[strlen(source)-1] == '>' ){
		int path_size = strlen(source)-2;
		return path_size;
	}
	return -1;
}
int parsePath(char* source, char* path){
	
	if (source[0] == '<' && source[1] != '/' &&source[strlen(source)-1] == '>' ){
		int path_size = strlen(source)-2;
		strncpy(path, source+1, path_size);
		path[path_size] = '\0';
		return 1;
	}
	return 0;
}

int optionRegister(coap_resource_t **resource , char* temp_str){
	
	int type_size, data_size;
	int upper_status = calOptionSize(temp_str,&type_size, &data_size);
	if (upper_status && type_size > 0 && data_size > 0){
		char* opt_type = malloc(sizeof(char) * (type_size+1));
		char* opt_data = malloc(sizeof(char) * (data_size+1));
		int status = parseOption(temp_str, opt_type,opt_data);
		if(status){
			coap_add_attr(*resource, opt_type, strlen(opt_type), opt_data, strlen(opt_data), 0);
		}
		else {
			free(opt_type);
			free(opt_data);
		}
		// TODO:free resource attr if status error
		return status;
	}
	return 0;
}

int pathRegister(coap_resource_t *new_resource, coap_resource_t **resource , char* temp_str){
	
	if (calPathSize(temp_str) > 0){
		int total_size = new_resource->uri.length + calPathSize(temp_str);
		char* rel_path = malloc(sizeof(char) * (calPathSize(temp_str)+1)); 
		int status = parsePath(temp_str, rel_path);
		if(status){
			char* abs_path = malloc(sizeof(char) * (total_size + 1));
			sprintf(abs_path,"%s/%s", new_resource->uri.s, rel_path);
			*resource = coap_resource_init(abs_path, strlen(abs_path), 0); 
		}
		free(rel_path);
		return status;
	}
	return 0;
}

int parseLinkFormat(char* str, coap_resource_t* old_resource, coap_resource_t** resource){ 
	  char * pch;
	  int last_counter = 0;
	  int counter = 0;
	  
	  pch=strchr(str,';');
	  while (pch!=NULL)
	  {
		counter = pch-str;
		
		int size = counter-last_counter;
		
		// first element of link format
		if(str[last_counter] != ';'){
			
			char* temp_str = malloc(sizeof(char) * (size+1));
			strncpy(temp_str, str+last_counter, size);
			temp_str[size] = '\0'; 
				int status = pathRegister(old_resource, resource,temp_str);
			free(temp_str);
			if (!status) return 0;
		}
		
		// in between first and last element of link format
		else{
			char* temp_str = malloc(sizeof(char) * (size));
			strncpy(temp_str, str+last_counter+1, size-1);
			temp_str[size-1] = '\0';
				int status = optionRegister(resource,temp_str); 
			//TODO : fix this free(), somehow it works now
			free(temp_str);
			if (!status) return 0;
		}
		
		// last element of link format
		pch=strchr(pch+1,';');
		if (pch == NULL){
			size = strlen(str+counter+1);
			char* temp_str = malloc(sizeof(char) * (size+1));
			strncpy(temp_str, str+counter+1, size);
			temp_str[size] = '\0';
				int status = optionRegister(resource,temp_str); 
			//TODO : fix this free(), somehow it works now
			free(temp_str); 
			if (!status) return 0;
		}
				
		last_counter = counter;
	  }
	  return 1;
}

	//char str[] ="<sensors>;if=\"temperature-c\";rt=\"sensor;ct=45;title=\"sensor\"";
	//parseLinkFormat(str, &pathRegister, &optionRegister);



/* Link Format Parser ends here */

#endif

#ifdef TEMPMODE
	TopicDataPtr topicDB = NULL; /* initially there are no nodes */
#endif
 
static void hnd_get_topic(coap_context_t *ctx, struct coap_resource_t *resource, 
              const coap_endpoint_t *local_interface, coap_address_t *peer, 
              coap_pdu_t *request, str *token, coap_pdu_t *response);

static void hnd_put_topic(coap_context_t *ctx ,
             struct coap_resource_t *resource ,
             const coap_endpoint_t *local_interface ,
             coap_address_t *peer ,
             coap_pdu_t *request,
             str *token ,
             coap_pdu_t *response);
             
static void hnd_delete_topic(coap_context_t *ctx ,
                struct coap_resource_t *resource ,
                const coap_endpoint_t *local_interface ,
                coap_address_t *peer ,
                coap_pdu_t *request ,
                str *token ,
                coap_pdu_t *response ); 
                
static void hnd_post_topic(coap_context_t *ctx ,
                struct coap_resource_t *resource ,
                const coap_endpoint_t *local_interface ,
                coap_address_t *peer ,
                coap_pdu_t *request ,
                str *token ,
                coap_pdu_t *response );
             
static void hnd_get_broker(coap_context_t *ctx, struct coap_resource_t *resource, 
              const coap_endpoint_t *local_interface, coap_address_t *peer, 
              coap_pdu_t *request, str *token, coap_pdu_t *response);
              
static void hnd_post_broker(coap_context_t *ctx,
             struct coap_resource_t *resource,
             const coap_endpoint_t *local_interface,
             coap_address_t *peer,
             coap_pdu_t *request,
             str *token,
             coap_pdu_t *response);


int main(int argc, char* argv[])
{
	coap_context_t*  ctx;
	coap_address_t   serv_addr;
	coap_resource_t* broker_resource;
	fd_set           readfds;    
	char broker_path[8] = "ps";
	
	/* turn on debug an printf() */
	coap_log_t log_level = LOG_DEBUG;
	coap_set_log_level(log_level);
	/* turn on debug an printf() */
		
	/* Prepare the CoAP server socket */ 
	coap_address_init(&serv_addr);
	serv_addr.addr.sin.sin_family      = AF_INET6;
	serv_addr.addr.sin.sin_addr.s_addr = INADDR_ANY;
	serv_addr.addr.sin.sin_port        = htons(5683); //default port
	ctx                                = coap_new_context(&serv_addr);
	if (!ctx) exit(EXIT_FAILURE);
	/* Prepare the CoAP server socket */ 	
	
	/* Initialize the observable resource */
	broker_resource = coap_resource_init(broker_path, strlen(broker_path), 0);
	coap_register_handler(broker_resource, COAP_REQUEST_GET, hnd_get_broker);
	coap_register_handler(broker_resource, COAP_REQUEST_POST, hnd_post_broker);
	coap_add_attr(broker_resource, (unsigned char *)"ct", 2, (unsigned char *)"40", strlen("40"), 0); //40 :link
	coap_add_attr(broker_resource, (unsigned char *)"rt", 2, (unsigned char *)"core.ps", strlen("core.ps"), 0);
	coap_add_resource(ctx, broker_resource);
	/* Initialize the observable resource */	
	
	
	
	/*Listen for incoming connections*/	
	while (1) {
        FD_ZERO(&readfds);
        FD_SET( ctx->sockfd, &readfds );
        /* Block until there is something to read from the socket */
        int result = select( FD_SETSIZE, &readfds, 0, 0, NULL );
        if ( result < 0 ) {         /* error */
            perror("select");
			exit(EXIT_FAILURE);
        } else if ( result > 0 ) {  /* read from socket */
            if ( FD_ISSET( ctx->sockfd, &readfds ) ) 
                coap_read( ctx );       
        } 
        
        // coap_check_notify is needed if there is a observable resource
        coap_check_notify(ctx);
    }
    
    coap_free_context(ctx);  
}

static void
hnd_get_broker(coap_context_t *ctx, struct coap_resource_t *resource, 
              const coap_endpoint_t *local_interface, coap_address_t *peer, 
              coap_pdu_t *request, str *token, coap_pdu_t *response) 
{
	unsigned char buf[3];
	unsigned char response_data[1024];
	size_t response_size = sizeof(response_data);
	size_t response_offset = 0;
	RESOURCES_ITER(ctx->resources, r) {
	response_size = sizeof(response_data);
	response_offset = 0;
	coap_print_link(r, response_data, &response_size, &response_offset);
  }
  

		response->hdr->code 		  = COAP_RESPONSE_CODE(205); 
		// option order matters!
		coap_add_option(response, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);
		coap_add_option(response, COAP_OPTION_MAXAGE,coap_encode_var_bytes(buf, 20), buf); // mac-age in seconds, so 30 seconds (to mars. #pun)
		coap_add_data  (response, response_size, response_data);
		//coap_add_data  (response, resource->uri.length, resource->uri.s);		
}

static void
hnd_post_broker(coap_context_t *ctx, struct coap_resource_t *resource, 
              const coap_endpoint_t *local_interface, coap_address_t *peer, 
              coap_pdu_t *request, str *token, coap_pdu_t *response) 
{
	coap_resource_t *new_resource;
	size_t size;
    unsigned char *data;
    unsigned char *data_safe;

	(void)coap_get_data(request, &size, &data);
	data_safe = coap_malloc(sizeof(char)*(size+1));
	sprintf(data_safe, "%s", data);
	//int status = 0;
	int status = parseLinkFormat(data_safe,resource, &new_resource);
	
	
	response->hdr->code = status ? COAP_RESPONSE_CODE(201) : COAP_RESPONSE_CODE(400);
	if (status){
		coap_register_handler(new_resource, COAP_REQUEST_GET, hnd_get_topic);
		coap_register_handler(new_resource, COAP_REQUEST_POST, hnd_post_topic);
		coap_register_handler(new_resource, COAP_REQUEST_PUT, hnd_put_topic);
		coap_register_handler(new_resource, COAP_REQUEST_DELETE, hnd_delete_topic);
		new_resource->observable = 1;
		coap_add_resource(ctx, new_resource);
		coap_add_option(response, COAP_OPTION_LOCATION_PATH, new_resource->uri.length, new_resource->uri.s);
		addTopicWEC(&topicDB, new_resource->uri.s, new_resource->uri.length, time(NULL));
	}
	coap_free(data_safe);
}

static void hnd_get_topic(coap_context_t *ctx, struct coap_resource_t *resource, 
              const coap_endpoint_t *local_interface, coap_address_t *peer, 
              coap_pdu_t *request, str *token, coap_pdu_t *response){
					
		unsigned char buf[3];
		 
		TopicDataPtr temp = getTopic(&topicDB, resource->uri.s);
		if (temp == NULL || temp->data == NULL) {
			response->hdr->code 	= COAP_RESPONSE_CODE(404);
		}
		else {
			response->hdr->code 	= COAP_RESPONSE_CODE(205);
			coap_add_option(response, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);
			coap_add_option(response, COAP_OPTION_MAXAGE,coap_encode_var_bytes(buf, 20), buf); // mac-age in seconds, so 30 seconds (to mars. #pun)
			coap_add_data  (response, strlen(temp->data), temp->data);
		}
		// option order matters!

}

static void hnd_put_topic(coap_context_t *ctx ,
             struct coap_resource_t *resource ,
             const coap_endpoint_t *local_interface ,
             coap_address_t *peer ,
             coap_pdu_t *request,
             str *token ,
             coap_pdu_t *response){
				 
	size_t size;
    unsigned char *data; 
	(void)coap_get_data(request, &size, &data);
	int status = updateTopicData(&topicDB, resource->uri.s, 0, data, size);
	response->hdr->code = status ? COAP_RESPONSE_CODE(201) : COAP_RESPONSE_CODE(400);
}
            
static void hnd_delete_topic(coap_context_t *ctx ,
                struct coap_resource_t *resource ,
                const coap_endpoint_t *local_interface ,
                coap_address_t *peer ,
                coap_pdu_t *request ,
                str *token ,
                coap_pdu_t *response ){
					
	int status = deleteTopic(&topicDB, resource->uri.s);
	/* FIXME: link attributes for resource have been created dynamically
	* using coap_malloc() and must be released. */
	if (status){
		coap_delete_resource(ctx, resource->key);
		response->hdr->code = COAP_RESPONSE_CODE(202);
	}
}
                
static void hnd_post_topic(coap_context_t *ctx ,
                struct coap_resource_t *resource ,
                const coap_endpoint_t *local_interface ,
                coap_address_t *peer ,
                coap_pdu_t *request ,
                str *token ,
                coap_pdu_t *response ){
					coap_resource_t *new_resource;
	size_t size;
    unsigned char *data;
    unsigned char *data_safe;

	(void)coap_get_data(request, &size, &data);
	data_safe = coap_malloc(sizeof(char)*(size+1));
	sprintf(data_safe, "%s", data);
	//int status = 0;
	int status = parseLinkFormat(data_safe,resource, &new_resource);
	
	
	response->hdr->code = status ? COAP_RESPONSE_CODE(201) : COAP_RESPONSE_CODE(400);
	if (status){
		coap_register_handler(new_resource, COAP_REQUEST_GET, hnd_get_topic);
		coap_register_handler(new_resource, COAP_REQUEST_POST, hnd_post_topic);
		coap_register_handler(new_resource, COAP_REQUEST_PUT, hnd_put_topic);
		coap_register_handler(new_resource, COAP_REQUEST_DELETE, hnd_delete_topic);
		new_resource->observable = 1;
		coap_add_resource(ctx, new_resource);
		coap_add_option(response, COAP_OPTION_LOCATION_PATH, new_resource->uri.length, new_resource->uri.s);
		addTopicWEC(&topicDB, new_resource->uri.s, new_resource->uri.length, time(NULL));
	}
	coap_free(data_safe);
}

