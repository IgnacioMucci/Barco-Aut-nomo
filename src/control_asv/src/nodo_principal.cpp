#include "rclcpp/rclcpp.hpp"

class ControlASV : public rclcpp::Node
{
public:
  ControlASV() : Node("control_asv_principal")
  {
    RCLCPP_INFO(this->get_logger(), "Iniciando nodo principal del ASV. Sistema listo para integrar con Pixhawk.");
  }
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ControlASV>());
  rclcpp::shutdown();
  return 0;
}