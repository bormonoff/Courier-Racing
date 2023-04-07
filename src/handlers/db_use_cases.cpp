
#include "handlers/db_use_cases.h"

namespace http_handler {

Leaders UseCases::GetLeaders() {
    return dogs_.GetLeaders();
}

}  // namespace http_handler