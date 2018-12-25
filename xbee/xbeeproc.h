/*
 * xbeeproc.h
 *
 *  Created on: Dec 22, 2018
 *      Author: linux
 */

#ifndef RADIO_XBEEPROC_H_
#define RADIO_XBEEPROC_H_

// DEFINES
#define INPUT  1
#define OUTPUT 2
#define BOTH   3

// EXTERNALS
extern pthread_t xbeetask;
extern void *XbeeTask(void *value);

namespace std
{

} /* namespace std */
#endif /* RADIO_XBEEPROC_H_ */
