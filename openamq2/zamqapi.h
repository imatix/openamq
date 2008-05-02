//  ZAMQ API functions
//  Part of OpenAMQ/2.0 WireAPI layer

#   ifdef __cplusplus
extern "C" {
#   endif

    int zamq_open    (char *host);
    int zamq_close   (void);
    int zamq_send    (void *content, size_t length);
    int zamq_receive (void *content, size_t max_length, int block);

#   ifdef __cplusplus
}
#   endif

