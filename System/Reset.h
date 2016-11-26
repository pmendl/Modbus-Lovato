#ifndef RESET_H
#define RESET_H

namespace System {

void initiateReset(void);
bool startResetSensitiveProcess(int priority);
void endResetSensitiveProcess(int priority);

} // System

#endif // RESET_H
