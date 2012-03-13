/*
 * crc.h -- compute CRC for Davis Instruments data interface
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _CRC_H
#define _CRC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned short	crc_t;

extern void	crc_addbyte(crc_t *crc, unsigned char c);
extern crc_t	*crc_new(void);
extern void	crc_free(crc_t *crc);
extern void	crc_start(crc_t *crc);
extern unsigned short	crc_stop(crc_t *crc);
extern int	crc_cmp(crc_t *crc1, crc_t *crc2);
extern int	crc_check(crc_t *crc, unsigned char *buffer, int len);

#ifdef __cplusplus
}
#endif

#endif /* _CRC_H */
