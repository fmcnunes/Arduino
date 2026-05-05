#ifndef ARDPRINTF
#define ARDPRINTF
#define ARDBUFFER 300 //Buffer for storing intermediate strings. Performance may vary depending on size.
#include <stdarg.h>
#include <Arduino.h> //To allow function to run from any file in a project


#define FLASHLOGBUFFER 300

char temp[ARDBUFFER+1];

int log(const __FlashStringHelper* flashStr, ...)
{
  
  char str[FLASHLOGBUFFER];
  int i, count=0, j=0, flag=0;

  Fcopy(str, FLASHLOGBUFFER,flashStr);
  
//  Serial.print(getDateTime());
//  Serial.print(F("\t"));
  
  for(i=0; str[i]!='\0';i++)  if(str[i]=='%')  count++; //Evaluate number of arguments required to be printed

  if (count==0)
  {
    Serial.println(str);
    return 0;
  }
  va_list argv;
  va_start(argv, count);
  for(i=0,j=0; str[i]!='\0';i++) //Iterate over formatting string
  {
    if(str[i]=='%')
    {
      //Clear buffer
      temp[j] = '\0'; 
      Serial.print(temp);
      j=0;
      temp[0] = '\0';
      
      //Process argument
      switch(str[++i])
      {
        case 'd': Serial.print(va_arg(argv, int));
                  break;
        case 'l': Serial.print(va_arg(argv, long));
                  break;
        case 'f': Serial.print(va_arg(argv, double));
                  break;
        case 'c': Serial.print((char)va_arg(argv, int));
                  break;
        case 's': Serial.print(va_arg(argv, char *));
                  break;
        default:  ;
      };
    }
    else 
    {
      //Add to buffer
      temp[j] = str[i];
      j = (j+1)%ARDBUFFER;
      if(j==0)  //If buffer is full, empty buffer.
      {
        temp[ARDBUFFER] = '\0';
        Serial.print(temp);
        temp[0]='\0';
      }
    }
  };

  temp[j] = '\0'; 
  Serial.println(temp); //Print trailing newline
  
  return count + 1; //Return number of arguments detected
}

int log(char *str, ...)
{
  int i, count=0, j=0, flag=0;

  Serial.print(getDateTime());
  Serial.print(F("\t"));
  
  for(i=0; str[i]!='\0';i++)  if(str[i]=='%')  count++; //Evaluate number of arguments required to be printed

  if (count==0)
  {
    Serial.println(str);
    return 0;
  }
  va_list argv;
  va_start(argv, count);
  for(i=0,j=0; str[i]!='\0';i++) //Iterate over formatting string
  {
    if(str[i]=='%')
    {
      //Clear buffer
      temp[j] = '\0'; 
      Serial.print(temp);
      j=0;
      temp[0] = '\0';
      
      //Process argument
      switch(str[++i])
      {
        case 'd': Serial.print(va_arg(argv, int));
                  break;
        case 'l': Serial.print(va_arg(argv, long));
                  break;
        case 'f': Serial.print(va_arg(argv, double));
                  break;
        case 'c': Serial.print((char)va_arg(argv, int));
                  break;
        case 's': Serial.print(va_arg(argv, char *));
                  break;
        default:  ;
      };
    }
    else 
    {
      //Add to buffer
      temp[j] = str[i];
      j = (j+1)%ARDBUFFER;
      if(j==0)  //If buffer is full, empty buffer.
      {
        temp[ARDBUFFER] = '\0';
        Serial.print(temp);
        temp[0]='\0';
      }
    }
  };

  temp[j] = '\0'; 
  Serial.println(temp); //Print trailing newline
  
  return count + 1; //Return number of arguments detected
}

#undef ARDBUFFER
#endif

