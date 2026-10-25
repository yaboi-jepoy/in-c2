#pragma once

#include "types.h"

void cloudInit();
bool cloudUpload(const FormattedRecord& record);
bool backupRecord(const FormattedRecord& record);
void flushBackup();
void cloudService();