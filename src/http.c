//
// Created by chord on 18-5-7.
//

#include "http.h"

ptt_mime_type_t ptt_mime[] =
        {
                {".html", "text/html"},
                {".txt", "text/plain"},
                {".png", "image/png"},
                {".gif", "image/gif"},
                {".jpg", "image/jpeg"},
                {".jpeg", "image/jpeg"},
                {NULL ,"text/plain"}
        };

void ptt_do_request(void *args)
{

}