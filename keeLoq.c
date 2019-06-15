/*keeLoq加密解密算法*/


#define bit(x,n)                        (((x)>>(n))&1)
#define g5(x,a,b,c,d,e)                 (bit(x,a)+bit(x,b)*2+bit(x,c)*4+bit(x,d)*8+bit(x,e)*16)


int keeLoq_decrypt(const uint8_t *in,int inlen,uint32_t nlf,uint8_t *out)
{
    if(in == NULL || out == NULL || inlen <4) {
        return 0;
    }

    if(0 != (inlen%4)) {
        printf("int is not 4-byte alignment\n");
        return 0;
    }
        
    int i=0;
    uint32_t dat;
    const uint64_t key =0x1234567890123456;
    while(inlen>i) {
        dat = (in[i]<<24) | (in[i+1]<<16) | (in[i+2]<<8) | in[i+3];
        for(uint32_t r=0;r<528;r++) {
            dat = (dat<<1)^bit(dat,31)^bit(dat,15)^(uint32_t)bit(key,(15-r)&63)^bit(nlf,g5(dat,0,8,19,25,30));
        }
        out[i++] = (dat >> 24);
        out[i++] = (dat >> 16);
        out[i++] = (dat >> 8);
        out[i++] = (uint8_t)dat;
    }
    return inlen;
}

int keeLoq_encrypt(const uint8_t *in,int inlen,uint32_t nlf,uint8_t *out)
{
    if(in==NULL || out==NULL) {
        return 0;
    }

    if(0 != (inlen%4)) {
        printf("int is not 4-byte alignment\n");
        return 0;
    }
        
    int i = 0;
    uint32_t dat;
    const uint64_t key =0x1234567890123456;
    while(inlen>i) {
        dat = (in[i]<<24) | (in[i+1]<<16) | (in[i+2]<<8) | in[i+3];
        for (uint32_t r = 0; r < 528; r++) {
            dat = (dat>>1)^((bit(dat,0)^bit(dat,16)^(uint32_t)bit(key,r&63)^bit(nlf,g5(dat,1,9,20,26,31)))<<31);
        }
        out[i++] = (dat >> 24);
        out[i++] = (dat >> 16);
        out[i++] = (dat >> 8);
        out[i++] = (uint8_t)dat;
    }
    return inlen;
}
