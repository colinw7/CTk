#ifndef CTkAppCmdNotify_H
#define CTkAppCmdNotify_H

class CTkAppCmdNotify {
 public:
  CTkAppCmdNotify() { }

  virtual ~CTkAppCmdNotify() { }

  virtual void handleRename(const char *oldName, const char *newName) = 0;
  virtual void handleDelete(const char *name) = 0;
};

#endif
