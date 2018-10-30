/*
  Copyright (c) 2009 Dave Gamble

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

/* cJSON */
/* JSON parser in C. */

#include "MMI_features.h"

#include "cJSON.h"
#include "med_smalloc.h"
#include "Math.h"       /* pow */
#include "med_utility.h"

//#include "stdarg.h"     /*可变参数函数*/

static const char *ep=NULL;

static void *(*cJSON_malloc)(U32 sz) = mtk_malloc;
static void (*cJSON_free)(void **ptr) = mtk_mfree;

void *mtk_malloc(U32 size)
{
    return med_alloc_ext_mem((kal_int32)size);
}

void mtk_mfree(void **p)
{
	med_free_ext_mem(p);
}

const char *cJSON_GetErrorPtr(void)
{    
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    return ep;
}

int cJSON_charTolower(int c)  
{  
    if (c >= 'A' && c <= 'Z')  
    {  
        return c + 'a' - 'A';  
    }  
    else  
    {  
        return c;  
    }  
} 

static int cJSON_strcasecmp(const char *s1,const char *s2)
{
    
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
	if (!s1)
	{
        return (s1==s2)?0:1;
	}
    if (!s2) 
    {
        return 1;
    }
	for(; cJSON_charTolower(*s1)==cJSON_charTolower(*s2); ++s1, ++s2)	
	{
        if(*s1 == 0)
        {
            return 0;
        }
	}
	return cJSON_charTolower(*(const unsigned char *)s1) - cJSON_charTolower(*(const unsigned char *)s2);
}

static char* cJSON_strdup(const char* str)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U32 len = 0;
    char *copy = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    if(NULL == str)
    {
        return NULL;
    }
    
    len = strlen(str) + 1;
    if (!(copy = (char*)cJSON_malloc(len))) 
    {
        return NULL;
    }
    memcpy(copy,str,len);
    return copy;
}

void cJSON_InitHooks(cJSON_Hooks* hooks)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    if (!hooks) 
    {
        /* Reset hooks */
        cJSON_malloc = mtk_malloc;	
        cJSON_free = mtk_mfree;
        return;
    }

	cJSON_malloc = (hooks->malloc_fn)?hooks->malloc_fn:mtk_malloc;
	cJSON_free	 = (hooks->free_fn)?hooks->free_fn:mtk_mfree;
}

/* Internal constructor. */
static cJSON *cJSON_New_Item(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	cJSON* node = (cJSON*)cJSON_malloc(sizeof(cJSON));

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    
	if (node) 
	{
        memset(node,0x00,sizeof(cJSON));
	}
    
	return node;
}

/* Delete a cJSON structure. */
void cJSON_Delete(cJSON *c)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	cJSON *next=NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    if(NULL == c)
    {
        return;
    }
    
	while (c)
	{
		next=c->next;
		if (!(c->type&cJSON_IsReference) && c->child)
		{
            cJSON_Delete(c->child);
		}
		if (!(c->type&cJSON_IsReference) && c->valuestring)
		{
            cJSON_free(&(c->valuestring));
		}
		if (c->string)
		{
            cJSON_free(&(c->string));
		}
		cJSON_free((void**)&c);
		c=next;
	}
}

/* Parse the input text to generate a number, and populate the result into item. */
static const char *parse_number(cJSON *item,const char *num)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	double n=0,sign=1,scale=0;
    int subscale=0,signsubscale=1;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(NULL==item || NULL==num)
    {
        return NULL;
    }
	if (*num=='-') /* Has sign? */
	{
        sign=-1;
        num++;  
	}
	if (*num=='0') /* is zero */
	{
        num++;          
	}
	if (*num>='1' && *num<='9') /* Number? */
	{
        do
        {
            n=(n*10.0)+(*num++ -'0');   
        }while (*num>='0' && *num<='9');
	}
	if (*num=='.' && num[1]>='0' && num[1]<='9')/* Fractional part? */
    {
        num++;
        do
        {
            n=(n*10.0)+(*num++ -'0');
            scale--; 
        }while (*num>='0' && *num<='9');
    }  
	if (*num=='e' || *num=='E')		/* Exponent? */
	{	
        num++;
        if (*num=='+') 
        {
            num++; 
        }
        else if (*num=='-')  /* With sign? */
        {
            signsubscale=-1;
            num++;
        }
        
		while (*num>='0' && *num<='9')/* Number? */
		{
            subscale=(subscale*10)+(*num++ - '0'); 
        }
	}

	n=sign*n*pow(10.0,(scale+subscale*signsubscale));	/* number = +/- number.fraction * 10^+/- exponent */
	
	item->valuedouble=n;
	item->valueint=(int)n;
	item->type=cJSON_Number;
	return num;
}

/* Render the number nicely from the given item into a string. */
static char *print_number(cJSON *item)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	char *str = NULL;
	double d=item->valuedouble;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    if(NULL == item)
    {
        return NULL;
    }
	if (fabs(((double)item->valueint)-d)<=DBL_EPSILON && d<=INT_MAX && d>=INT_MIN)
	{
		str=(char*)cJSON_malloc(21);	/* 2^64+1 can be represented in 21 chars. */
		if (str) 
		{
            sprintf(str,"%d",item->valueint);
		}
	}
	else
	{
		str=(char*)cJSON_malloc(64);	/* This is a nice tradeoff. */
		if (str)
		{
			if (fabs(floor(d)-d)<=DBL_EPSILON && fabs(d)<1.0e60)
			{
                sprintf(str,"%.0f",d);
			}
			else if (fabs(d)<1.0e-6 || fabs(d)>1.0e9)
			{
                sprintf(str,"%e",d);
			}
			else
			{
                sprintf(str,"%f",d);
			}
		}
	}
	return str;
}

static unsigned parse_hex4(const char *str)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	unsigned h=0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    
	if (*str>='0' && *str<='9')
	{
        h+=(*str)-'0'; 
	}
    else if (*str>='A' && *str<='F')
    {
        h+=10+(*str)-'A'; 
    }
    else if (*str>='a' && *str<='f')
    {
        h+=10+(*str)-'a'; 
    }
    else
    {
        return 0;
    }
    
	h=h<<4;str++;
	if (*str>='0' && *str<='9') 
	{
        h+=(*str)-'0'; 
	}
    else if (*str>='A' && *str<='F') 
    {
        h+=10+(*str)-'A'; 
    }
    else if (*str>='a' && *str<='f') 
    {
        h+=10+(*str)-'a';
    }
    else 
    {
        return 0;
    }
    
	h=h<<4;str++;
	if (*str>='0' && *str<='9') 
	{
        h+=(*str)-'0'; 
	}
    else if (*str>='A' && *str<='F') 
    {
        h+=10+(*str)-'A';
    }
    else if (*str>='a' && *str<='f') 
    {
        h+=10+(*str)-'a'; 
    }
    else
    {
        return 0;
    }
    
	h=h<<4;str++;
	if (*str>='0' && *str<='9') 
	{
        h+=(*str)-'0'; 
	}
    else if (*str>='A' && *str<='F')
    {
        h+=10+(*str)-'A'; 
    }
    else if (*str>='a' && *str<='f')
    {
        h+=10+(*str)-'a'; 
    }
    else
    {
        return 0;
    }
	return h;
}

/* Parse the input text into an unescaped cstring, and populate item. */
static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
static const char *parse_string(cJSON *item,const char *str)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	const char *ptr=str+1;
    char *ptr2=NULL, *out=NULL;
    int len=0;
    unsigned uc=0,uc2=0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    if(NULL==item || NULL==str)
    {
        return NULL;
    }
    
	if (*str!='\"')/* not a string! */
    {
        ep=str;
        return NULL;
    }
	
	while (*ptr!='\"' && *ptr && ++len)  /* Skip escaped quotes. */
	{
        if (*ptr++ == '\\') 
        {
            ptr++; 
        }
	}
	
	out=(char*)cJSON_malloc(len+1);	/* This is how long we need for the string, roughly. */
	if (!out) 
	{
        return NULL;
	}
    
	ptr=str+1;
    ptr2=out;
	while (*ptr!='\"' && *ptr)
	{
		if (*ptr!='\\') 
		{
            *ptr2++=*ptr++;
		}
		else
		{
			ptr++;
			switch (*ptr)
			{
				case 'b': 
                {
                    *ptr2++='\b';   
                    break;
				}
				case 'f':
                {
                    *ptr2++='\f';
                    break;
				}
				case 'n':
                {
                    *ptr2++='\n'; 
                    break;
				}
                case 'r':
                {
                    *ptr2++='\r';
                    break;
                }
				case 't': 
                {
                    *ptr2++='\t'; 
                    break;
				}
				case 'u':	 /* transcode utf16 to utf8. */
                {
					uc=parse_hex4(ptr+1);ptr+=4;	/* get the unicode char. */

					if ((uc>=0xDC00 && uc<=0xDFFF) || uc==0)	 /* check for invalid.   */
					{
                        break; 
					}
					if (uc>=0xD800 && uc<=0xDBFF)	/* UTF16 surrogate pairs.	*/
					{
						if (ptr[1]!='\\' || ptr[2]!='u') /* missing second-half of surrogate.    */
						{
                            break; 
						}
						uc2=parse_hex4(ptr+3);
                        ptr+=6;
						if (uc2<0xDC00 || uc2>0xDFFF)	  /* invalid second-half of surrogate.    */
						{
                            break;
						}
						uc=0x10000 + (((uc&0x3FF)<<10) | (uc2&0x3FF));
					}

					len=4;
                    if (uc<0x80) 
                    {
                        len=1;
                    }
                    else if (uc<0x800)
                    {
                        len=2;
                    }
                    else if (uc<0x10000)
                    {
                        len=3; 
                    }

                    ptr2+=len;
					
					switch (len) 
                    {
						case 4: 
                        {
                            *--ptr2 =((uc | 0x80) & 0xBF);
                            uc >>= 6;
						}
						case 3:
                        {
                            *--ptr2 =((uc | 0x80) & 0xBF);
                            uc >>= 6;
						}
						case 2:
                        {
                            *--ptr2 =((uc | 0x80) & 0xBF); 
                            uc >>= 6;
						}
						case 1: 
                        {
                            *--ptr2 =(uc | firstByteMark[len]);
						}
					}
					ptr2+=len;
					break;
				}
				default:  
                {
                    *ptr2++=*ptr; 
                    break;
				}
			}
			ptr++;
		}
	}
	*ptr2=0;
	if (*ptr=='\"')
	{
        ptr++;
	}
	item->valuestring=out;
	item->type=cJSON_String;
	return ptr;
}

/* Render the cstring provided to an escaped version that can be printed. */
static char *print_string_ptr(const char *str)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	const char *ptr=NULL;
    char *ptr2=NULL,*out=NULL;
    int len=0;
    unsigned char token = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    	
	if (!str) 
	{
        return cJSON_strdup("");
	}
	ptr=str;
    while ((token=*ptr) && ++len)
    {
        if (strchr("\"\\\b\f\n\r\t",token)) 
        {
            len++; 
        }
        else if (token<32)
        {
            len+=5;
        }
        ptr++;
    }
	
	out=(char*)cJSON_malloc(len+3);
	if (!out) 
	{
        return 0;
	}
	ptr2=out;
    ptr=str;
	*ptr2++='\"';
	while (*ptr)
	{
		if ((unsigned char)*ptr>31 && *ptr!='\"' && *ptr!='\\') 
		{
            *ptr2++=*ptr++;
		}
		else
		{
			*ptr2++='\\';
			switch (token=*ptr++)
			{
				case '\\':	
                {
                    *ptr2++='\\';   
                    break;
				}
				case '\"':	
                {
                    *ptr2++='\"';  
                    break;
				}
				case '\b':	
                {
                    *ptr2++='b';
                    break;
				}
				case '\f':	
                {
                    *ptr2++='f';
                    break;
				}
				case '\n':	
                {
                    *ptr2++='n';  
                    break;
				}
				case '\r':
                {
                    *ptr2++='r';
                    break;
				}
				case '\t':	
                {
                    *ptr2++='t';  
                    break;
				}
				default: 
                {
                    sprintf(ptr2,"u%04x",token);
                    ptr2+=5;    
                    break;  /* escape and print */
				}
			}
		}
	}
	*ptr2++='\"';
    *ptr2++=0;
	return out;
}
/* Invote print_string_ptr (which is useful) on an item. */
static char *print_string(cJSON *item)	
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    if(NULL == item)
    {
        return NULL;
    }
    return print_string_ptr(item->valuestring);
}

/* Predeclare these prototypes. */
static const char *parse_value(cJSON *item,const char *value);
static char *print_value(cJSON *item,int depth,int fmt);
static const char *parse_array(cJSON *item,const char *value);
static char *print_array(cJSON *item,int depth,int fmt);
static const char *parse_object(cJSON *item,const char *value);
static char *print_object(cJSON *item,int depth,int fmt);

/* Utility to jump whitespace and cr/lf */
static const char *skip(const char *in)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while (in && *in && (unsigned char)*in<=32) 
    {
        in++; 
    }
    return in;
}

/* Parse an object - create a new root, and populate. */
cJSON *cJSON_ParseWithOpts(const char *value,const char **return_parse_end,int require_null_terminated)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	const char *end=0;
	cJSON *c=cJSON_New_Item();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    
	if (!c || NULL==value)       /* memory fail */
	{
        return NULL; 
	}    
	ep=NULL;
    
	end=parse_value(c,skip(value));
	if (!end) /* parse failure. ep is set. */
    {
        cJSON_Delete(c);
        return NULL;
    }

	/* if we require null-terminated JSON without appended garbage, skip and then check for a null terminator */
	if (require_null_terminated) 
    {
        end=skip(end);
        if (*end)
        {
            cJSON_Delete(c);
            ep=end;
            return NULL;
        }
    }
	if (return_parse_end) 
	{
        *return_parse_end=end;
	}
	return c;
}
/* Default options for cJSON_Parse */
cJSON *cJSON_Parse(const char *value) 
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    if(NULL == value)
    {
        return NULL;
    }
    return cJSON_ParseWithOpts(value,0,0);
}

/* Render a cJSON item/entity/structure to text. */
char *cJSON_Print(cJSON *item)		
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/       
    return print_value(item,0,1);
}
char *cJSON_PrintUnformatted(cJSON *item)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/       
    return print_value(item,0,0);
}

/* Parser core - when encountering text, process appropriately. */
static const char *parse_value(cJSON *item,const char *value)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/       
	if (!value)	
	{
        return 0;   /* Fail on null. */
	}
	if (!strncmp(value,"null",4))	
    {
        item->type=cJSON_NULL;  
        return value+4; 
    }
	if (!strncmp(value,"false",5))
    { 
        item->type=cJSON_False; 
        return value+5; 
    }
	if (!strncmp(value,"true",4))
    { 
        item->type=cJSON_True;
        item->valueint=1;  
        return value+4; 
    }
	if (*value=='\"')		
    { 
        return parse_string(item,value); 
    }
	if (*value=='-' || (*value>='0' && *value<='9'))	
    { 
        return parse_number(item,value);
    }
	if (*value=='[')			
    { 
        return parse_array(item,value); 
    }
	if (*value=='{')		
    { 
        return parse_object(item,value); 
    }

	ep=value;
    return 0;	/* failure. */
}

/* Render a value to text. */
static char *print_value(cJSON *item,int depth,int fmt)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	char *out=0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/       
    
	if (!item) 
	{
        return 0;
	}
	switch ((item->type)&255)
	{
		case cJSON_NULL:	
        {
            out=cJSON_strdup("null");
            break;
		}
		case cJSON_False:	
        {
            out=cJSON_strdup("false");
            break;
		}
		case cJSON_True:	
        {
            out=cJSON_strdup("true");
            break;
		}
		case cJSON_Number:	
        {
            out=print_number(item);
            break;
		}
		case cJSON_String:	
        {
            out=print_string(item);
            break;
		}
		case cJSON_Array:	
        {
            out=print_array(item,depth,fmt);
            break;
		}
		case cJSON_Object:	
        {
            out=print_object(item,depth,fmt);
            break;
		}
	}
	return out;
}

/* Build an array from input text. */
static const char *parse_array(cJSON *item,const char *value)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	cJSON *child=NULL,*new_item = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/           
	if (*value!='[')	/* not an array! */
    {
        ep=value;
        return 0;
    }    

	item->type=cJSON_Array;
	value=skip(value+1);
	if (*value==']') /* empty array. */
	{
        return value+1; 
	}
	item->child=child=cJSON_New_Item();
	if (!item->child) /* memory fail */
	{
        return 0;        
	}
	value=skip(parse_value(child,skip(value)));	/* skip any spacing, get the value. */
	if (!value)
	{
        return 0;
	}
	while (*value==',')
	{
		if (!(new_item=cJSON_New_Item())) /* memory fail */
		{
            return 0;  
		}
		child->next=new_item;
        new_item->prev=child;
        child=new_item;
		value=skip(parse_value(child,skip(value+1)));
		if (!value)  /* memory fail */
		{
            return 0;  
		}
	}

	if (*value==']') 
	{
        return value+1; /* end of array */
	}
	ep=value;
    return 0;	/* malformed. */
}

/* Render an array to text */
static char *print_array(cJSON *item,int depth,int fmt)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	char **entries=NULL;
	char *out=NULL,*ptr=NULL,*ret=NULL;
	cJSON *child=item->child;
	int len=5,numentries=0,i=0,fail=0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/           	
	/* How many entries in the array? */
	while (child) 
	{
        numentries++;
        child=child->next;
	}
	/* Explicitly handle numentries==0 */
	if (!numentries)
	{
		out=(char*)cJSON_malloc(3);
		if (out) 
		{
            strcpy(out,"[]");
		}
		return out;
	}
	/* Allocate an array to hold the values for each */
	entries=(char**)cJSON_malloc(numentries*sizeof(char*));
	if (!entries) 
	{
        return 0;
	}
	memset(entries,0,numentries*sizeof(char*));
	/* Retrieve all the results: */
	child=item->child;
	while (child && !fail)
	{
		ret=print_value(child,depth+1,fmt);
		entries[i++]=ret;
		if (ret) 
		{
            len+=strlen(ret)+2+(fmt?1:0); 
		}
        else
        {
            fail=1;
        }
		child=child->next;
	}
	
	/* If we didn't fail, try to malloc the output string */
	if (!fail) 
	{
        out=(char*)cJSON_malloc(len);
	}
	/* If that fails, we fail. */
	if (!out) 
	{
        fail=1;
	}
	/* Handle failure. */
	if (fail)
	{
		for (i=0;i<numentries;i++) 
		{
            if (entries[i])
            {
                cJSON_free(&entries[i]);
            }
		}
		cJSON_free(entries);
		return 0;
	}
	
	/* Compose the output array. */
	*out='[';
	ptr=out+1;
    *ptr=0;
	for (i=0;i<numentries;i++)
	{
		strcpy(ptr,entries[i]);
        ptr+=strlen(entries[i]);
		if (i!=numentries-1) 
        {
            *ptr++=',';
            if(fmt)
            {
                *ptr++=' ';
            }
            *ptr=0;
        }
		cJSON_free(&entries[i]);
	}
	cJSON_free(entries);
	*ptr++=']';
    *ptr++=0;
	return out;	
}

/* Build an object from the text. */
static const char *parse_object(cJSON *item,const char *value)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	cJSON *child=NULL, *new_item=NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/               
	if (*value!='{')	/* not an object! */
    {
        ep=value;
        return 0;
    }    
	
	item->type=cJSON_Object;
	value=skip(value+1);
	if (*value=='}') /* empty array. */
	{
        return value+1; 
	}
	item->child=child=cJSON_New_Item();
	if (!item->child) 
	{
        return 0;
	}
	value=skip(parse_string(child,skip(value)));
	if (!value) 
	{
        return 0;
	}
	child->string=child->valuestring;
    child->valuestring=0;
	if (*value!=':') /* fail! */
    {
        ep=value;
        return 0;
    }
	value=skip(parse_value(child,skip(value+1)));	/* skip any spacing, get the value. */
	if (!value) 
	{
        return 0;
	}
	while (*value==',')
	{
		if (!(new_item=cJSON_New_Item()))/* memory fail */
		{
            return 0; 
		}
		child->next=new_item;
        new_item->prev=child;
        child=new_item;
		value=skip(parse_string(child,skip(value+1)));
		if (!value) 
		{
            return 0;
		}
		child->string=child->valuestring;
        child->valuestring=0;
		if (*value!=':') /* fail! */
        {
            ep=value;
            return 0;
        }
		value=skip(parse_value(child,skip(value+1)));	/* skip any spacing, get the value. */
		if (!value) 
		{
            return 0;
		}
	}
	
	if (*value=='}') /* end of array */
	{
        return value+1; 
	}
	ep=value;
    return 0;	/* malformed. */
}

/* Render an object to text. */
static char *print_object(cJSON *item,int depth,int fmt)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	char **entries=NULL,**names=NULL,*out=NULL,*ptr=NULL,*ret=NULL,*str=NULL;
	cJSON *child=item->child;
	int len=7,i=0,j=0,numentries=0,fail=0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/               
	/* Count the number of entries. */
	while (child) 
	{
        numentries++;
        child=child->next;
	}
	/* Explicitly handle empty object case */
	if (!numentries)
	{
		out=(char*)cJSON_malloc(fmt?depth+4:3);
		if (!out)	
		{
            return NULL;
		}
		ptr=out;
        *ptr++='{';
		if (fmt) 
        {
            *ptr++='\n';
            for (i=0;i<depth-1;i++)
            {
                *ptr++='\t';
            }
        }
		*ptr++='}';
        *ptr++=0;
		return out;
	}
	/* Allocate space for the names and the objects */
	entries=(char**)cJSON_malloc(numentries*sizeof(char*));
	if (!entries)
	{
        return 0;
    }
	names=(char**)cJSON_malloc(numentries*sizeof(char*));
	if (!names) 
    {
        cJSON_free(entries);
        return 0;
    }
	memset(entries,0,sizeof(char*)*numentries);
	memset(names,0,sizeof(char*)*numentries);

	/* Collect all the results into our arrays: */
	child=item->child;depth++;
    if (fmt) 
    {
        len+=depth;
    }
	while (child)
	{
		names[i]=str=print_string_ptr(child->string);
		entries[i++]=ret=print_value(child,depth,fmt);
		if (str && ret) 
		{
            len+=strlen(ret)+strlen(str)+2+(fmt?2+depth:0); 
		}
        else 
        {
            fail=1;
        }
		child=child->next;
	}
	
	/* Try to allocate the output string */
	if (!fail)
	{
        out=(char*)cJSON_malloc(len);
	}
	if (!out)
	{
        fail=1;
	}

	/* Handle failure */
	if (fail)
	{
		for (i=0;i<numentries;i++) 
        {
            if (names[i]) 
            {
                cJSON_free(&names[i]);
            }

            if (entries[i]) 
            {
                cJSON_free(&entries[i]);
            }
        }
		cJSON_free(names);cJSON_free(entries);
		return 0;
	}
	
	/* Compose the output: */
	*out='{';ptr=out+1;
    if (fmt)
    {
        *ptr++='\n';
    }
    *ptr=0;
	for (i=0;i<numentries;i++)
	{
		if (fmt) 
		{
            for (j=0;j<depth;j++) 
            {
                *ptr++='\t';
            }
		}
		strcpy(ptr,names[i]);
        ptr+=strlen(names[i]);
		*ptr++=':';
        if (fmt) 
        {
            *ptr++='\t';
        }
		strcpy(ptr,entries[i]);
        ptr+=strlen(entries[i]);
		if (i!=numentries-1)
		{
            *ptr++=',';
		}
		if (fmt) 
		{
            *ptr++='\n';
        }
        *ptr=0;
		cJSON_free(&names[i]);
        cJSON_free(&entries[i]);
	}
	
	cJSON_free(names);
    cJSON_free(entries);
	if (fmt) 
	{
        for (i=0;i<depth-1;i++) 
        {
            *ptr++='\t';
        }
	}
	*ptr++='}';
    *ptr++=0;
	return out;	
}

/* Get Array size/item / object item. */
int cJSON_GetArraySize(cJSON *array)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/    
    cJSON *c=NULL;
    int i=0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(NULL == array)
    {
        return 0;
    }

    c=array->child;
    
    while(c)
    {
        i++;
        c=c->next;
    }
    
    return i;
}

cJSON *cJSON_GetArrayItem(cJSON *array,int item)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    cJSON *c=NULL;      
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    if(NULL == array)
    {
        return NULL;
    }
    
    c=array->child;  
    while (c && item>0)
    {
        item--;
        c=c->next; 
    }
    return c;
}

cJSON *cJSON_GetObjectItem(cJSON *object,const char *string)	
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    cJSON *c=NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    
    if(NULL==object || NULL==object->child || NULL==string)
    {
        return NULL;
    }
    
    c = object->child;
    
    while (c && cJSON_strcasecmp(c->string,string))
    {
        c=c->next;
    }
    return c;
}

/* 
* 获取 cjson 对象的 item 的字符串值
*/
char *cJSON_GetObjectItem_string(cJSON *object,const char *string)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    cJSON *cjson_item=NULL;
	char *item_name = NULL;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    if(NULL == object || NULL==string)
    {
        return NULL;
    }

    cjson_item = cJSON_GetObjectItem(object, string);

    if(NULL==cjson_item)
    {
        return NULL;
    }
    
    return cjson_item->valuestring;
}

/* 
* 获取 cjson 对象的 item 的整数值
*/
int cJSON_GetObjectItem_integer(cJSON *object,const char *string)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    cJSON *cjson_item=NULL;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/ 
    
    if(NULL == object || NULL==string)
    {
        return 0;
    }

    cjson_item = cJSON_GetObjectItem(object, string);

    if(NULL==cjson_item)
    {
        return 0;
    }
    return cjson_item->valueint;
}

/* 
* 获取 cjson 对象的 item 的浮点值
*/
double cJSON_GetObjectItem_double(cJSON *object,const char *string)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    cJSON *cjson_item=NULL;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    if(NULL == object || NULL==string)
    {
        return 0.00;
    }

    cjson_item = cJSON_GetObjectItem(object, string);

    if(NULL==cjson_item)
    {
        return 0.00;
    }
    return cjson_item->valuedouble;
}

/* Utility for array list handling. */
static void suffix_object(cJSON *prev,cJSON *item) 
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    if(NULL==prev || NULL==item)
    {
        return;
    }
    
    prev->next=item;
    item->prev=prev;
}
/* Utility for handling references. */
static cJSON *create_reference(cJSON *item) 
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    cJSON *ref=cJSON_New_Item();
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    if (NULL==ref || NULL==item)
    {
        return NULL;
    }
    memcpy(ref,item,sizeof(cJSON));
    ref->string=0;
    ref->type|=cJSON_IsReference;
    ref->next=ref->prev=0;
    return ref;
}

/* Add item to array/object. */
void   cJSON_AddItemToArray(cJSON *array, cJSON *item)			
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    cJSON *c=array->child;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    
    if (!item)
    {
        return; 
    }
    if (!c) 
    {
        array->child=item;
    } 
    else
    {
        while (c && c->next)
        {
            c=c->next; 
        }
        suffix_object(c,item);
    }
}
void cJSON_AddItemToObject(cJSON *object,const char *string,cJSON *item)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/            
    if (!item) 
    {
        return; 
    }
    if (item->string) 
    {
        cJSON_free(&item->string);
    }
    item->string=cJSON_strdup(string);
    cJSON_AddItemToArray(object,item);
}
void cJSON_AddItemReferenceToArray(cJSON *array, cJSON *item)	
{
    
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    cJSON_AddItemToArray(array,create_reference(item));
}
void cJSON_AddItemReferenceToObject(cJSON *object,const char *string,cJSON *item)	
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    cJSON_AddItemToObject(object,string,create_reference(item));
}

cJSON *cJSON_DetachItemFromArray(cJSON *array,int which)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    cJSON *c=array->child;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    while (c && which>0) 
    {
        c=c->next;
        which--;
    }
    if (!c) 
    {
        return 0;
    }
	if (c->prev)
	{
        c->prev->next=c->next;
	}
    if (c->next) 
    {
        c->next->prev=c->prev;
    }
    if (c==array->child) 
    {
        array->child=c->next;
    }
    c->prev=c->next=0;
    return c;
}
void   cJSON_DeleteItemFromArray(cJSON *array,int which)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    cJSON_Delete(cJSON_DetachItemFromArray(array,which));
}
cJSON *cJSON_DetachItemFromObject(cJSON *object,const char *string)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i=0;
    cJSON *c=object->child;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    while (c && cJSON_strcasecmp(c->string,string))
    {
        i++,c=c->next;
    }
    if (c) 
    {
        return cJSON_DetachItemFromArray(object,i);
    }
    return 0;
}
void   cJSON_DeleteItemFromObject(cJSON *object,const char *string) 
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    cJSON_Delete(cJSON_DetachItemFromObject(object,string));
}

/* Replace array/object items with new ones. */
void cJSON_ReplaceItemInArray(cJSON *array,int which,cJSON *newitem)		
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    cJSON *c=array->child;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    while (c && which>0) 
    {
        c=c->next;
        which--;
    }
    if (!c)
    {
        return;
    }
	newitem->next=c->next;
    newitem->prev=c->prev;
    if (newitem->next) 
    {
        newitem->next->prev=newitem;
    }
	if (c==array->child) 
	{
        array->child=newitem; 
	}
    else 
    {
        newitem->prev->next=newitem;
    }
    c->next=c->prev=0;
    cJSON_Delete(c);
}
void   cJSON_ReplaceItemInObject(cJSON *object,const char *string,cJSON *newitem)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i=0;
    cJSON *c=object->child;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    while(c && cJSON_strcasecmp(c->string,string))
    {
        i++;
        c=c->next;
    }
    if(c)
    {
        newitem->string=cJSON_strdup(string);
        cJSON_ReplaceItemInArray(object,i,newitem);
    }
}

/* Create basic types: */
cJSON *cJSON_CreateNull(void)					
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    cJSON *item=cJSON_New_Item();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    if(item)
    {
        item->type=cJSON_NULL;
    }
    return item;
}
cJSON *cJSON_CreateTrue(void)					
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    cJSON *item=cJSON_New_Item();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    if(item)
    {
        item->type=cJSON_True;
    }
    return item;
}
cJSON *cJSON_CreateFalse(void)		
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    cJSON *item=cJSON_New_Item();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    if(item)
    {
        item->type=cJSON_False;
    }
    return item;
}
cJSON *cJSON_CreateBool(int b)		
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    cJSON *item=cJSON_New_Item();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    if(item)
    {
        item->type=b?cJSON_True:cJSON_False;
    }
    return item;
}
cJSON *cJSON_CreateNumber(double num)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    cJSON *item=cJSON_New_Item();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    if(item)
    {
        item->type=cJSON_Number;
        item->valuedouble=num;
        item->valueint=(int)num;
    }
    return item;
}
cJSON *cJSON_CreateString(const char *string)	
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    cJSON *item=cJSON_New_Item();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    if(item)
    {
        item->type=cJSON_String;
        item->valuestring=cJSON_strdup(string);
    }
    return item;
}
cJSON *cJSON_CreateArray(void)		
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    cJSON *item=cJSON_New_Item();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    if(item)
    {
        item->type=cJSON_Array;
    }
    return item;
}
cJSON *cJSON_CreateObject(void)		
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    cJSON *item=cJSON_New_Item();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    if(item)
    {
        item->type=cJSON_Object;
    }
    return item;
}

/* Create Arrays: */
cJSON *cJSON_CreateIntArray(const int *numbers,int count)	
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i=0;
    cJSON *n=0,*p=0,*a=cJSON_CreateArray();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    for(i=0;a && i<count;i++)
    {
        n=cJSON_CreateNumber(numbers[i]);
        if(!i)
        {
            a->child=n;
        }
        else 
        {
            suffix_object(p,n);
        }
        p=n;
    }
    return a;
}
cJSON *cJSON_CreateFloatArray(const float *numbers,int count)	
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i=0;
    cJSON *n=0,*p=0,*a=cJSON_CreateArray();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    for(i=0;a && i<count;i++)
    {
        n=cJSON_CreateNumber(numbers[i]);
        if(!i)
        {
            a->child=n;
        }
        else 
        {
            suffix_object(p,n);
        }
        p=n;
    }
    return a;
}
cJSON *cJSON_CreateDoubleArray(const double *numbers,int count)	
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i=0;
    cJSON *n=0,*p=0,*a=cJSON_CreateArray();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    for(i=0;a && i<count;i++)
    {
        n=cJSON_CreateNumber(numbers[i]);
        if(!i)
        {
            a->child=n;
        }
        else 
        {
            suffix_object(p,n);
        }
        p=n;
    }
    return a;
}
cJSON *cJSON_CreateStringArray(const char **strings,int count)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i=0;
    cJSON *n=0,*p=0,*a=cJSON_CreateArray();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/        
    for(i=0;a && i<count;i++)
    {
        n=cJSON_CreateString(strings[i]);
        if(!i)
        {
            a->child=n;
        }
        else
        {
            suffix_object(p,n);
        }
        p=n;
    }
    return a;
}

/* Duplication */
cJSON *cJSON_Duplicate(cJSON *item,int recurse)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	cJSON *newitem=NULL,*cptr=NULL,*nptr=NULL,*newchild=NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/            
	/* Bail on bad ptr */
	if (!item) 
	{
        return 0;
	}
	/* Create new item */
	newitem=cJSON_New_Item();
	if (!newitem) 
	{
        return 0;
	}
	/* Copy over all vars */
	newitem->type=item->type&(~cJSON_IsReference);
    newitem->valueint=item->valueint;
    newitem->valuedouble=item->valuedouble;
    
	if (item->valuestring)	
    {
        newitem->valuestring=cJSON_strdup(item->valuestring); 
        if (!newitem->valuestring)  
        {
            cJSON_Delete(newitem);
            return 0;
        }
    }
	if (item->string)		
    {
        newitem->string=cJSON_strdup(item->string);   
        if (!newitem->string) 
        {
            cJSON_Delete(newitem);
            return 0;
        }
    }
	/* If non-recursive, then we're done! */
	if (!recurse) 
	{
        return newitem;

	}
	/* Walk the ->next chain for the child. */
	cptr=item->child;
	while (cptr)
	{
		newchild=cJSON_Duplicate(cptr,1);		/* Duplicate (with recurse) each item in the ->next chain */
		if (!newchild) 
        {
            cJSON_Delete(newitem);
            return 0;
        }
		if (nptr)	 /* If newitem->child already set, then crosswire ->prev and ->next and move on */
        {
            nptr->next=newchild;
            newchild->prev=nptr;
            nptr=newchild;
        }   
		else		
        {
            newitem->child=newchild;
            nptr=newchild;
        }                    /* Set newitem->child and move to it */
		cptr=cptr->next;
	}
	return newitem;
}

void cJSON_Minify(char *json)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	char *into=json;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/            
    
	while (*json)
	{
		if (*json==' ') 
		{
            json++;
		}
		else if (*json=='\t') // Whitespace characters.
		{
            json++; 
		}
		else if (*json=='\r') 
		{
            json++;
		}
		else if (*json=='\n') 
		{
            json++;
		}
		else if (*json=='/' && json[1]=='/') // double-slash comments, to end of line.
		{
            while (*json && *json!='\n')
            {
                json++;    
            }
		}
		else if (*json=='/' && json[1]=='*')  // multiline comments.
        {
            while (*json && !(*json=='*' && json[1]=='/'))
            {
                json++;
            }
            json+=2;
        }   
		else if (*json=='\"')// string literals, which are \" sensitive.
        {
            *into++=*json++;
            while (*json && *json!='\"')
            {
                if (*json=='\\') 
                {
                    *into++=*json++;
                }
                *into++=*json++;
            }
            *into++=*json++;
        } 
		else       // All other characters.
		{
            *into++=*json++;      
		}
	}
	*into=0;	// and null-terminate.
}

