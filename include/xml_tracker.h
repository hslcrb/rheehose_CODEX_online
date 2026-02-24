#ifndef GITEX_XML_TRACKER_H
#define GITEX_XML_TRACKER_H

int xml_extract_semantic_signature(const char *path, char **signature);
int xml_diff_signatures(const char *old_sig, const char *new_sig, char **report);

#endif
