
#pragma once

#include <Models/Model.hpp>

class DexiNedModel : public Model {
public:
  void preProcess(PreProcessContext preContext);
  bool postProcess(PostProcessContext postContext);
  static std::string getModelName();
};