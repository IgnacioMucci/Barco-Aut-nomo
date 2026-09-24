/**
 * @file nodo_telemetria.cpp
 * @brief Nodo de ROS 2 que se suscribe a los tópicos de MAVROS para recuperar 
 *        telemetría de la Pixhawk (estado, batería, GPS, velocidad).
 */

#include "rclcpp/rclcpp.hpp"
#include "mavros_msgs/msg/state.hpp"
#include "sensor_msgs/msg/battery_state.hpp"
#include "sensor_msgs/msg/nav_sat_fix.hpp"
#include "geometry_msgs/msg/twist_stamped.hpp"

class NodoTelemetria : public rclcpp::Node
{
public:
  NodoTelemetria() : Node("nodo_telemetria")
  {
    // 1. Suscriptor para el estado general (Armado/Desarmado, Modo de vuelo)
    state_sub_ = this->create_subscription<mavros_msgs::msg::State>(
      "/mavros/state", 10, std::bind(&NodoTelemetria::state_cb, this, std::placeholders::_1));

    // 2. Suscriptor para el estado de la batería (Voltaje, Porcentaje)
    battery_sub_ = this->create_subscription<sensor_msgs::msg::BatteryState>(
      "/mavros/battery", 10, std::bind(&NodoTelemetria::battery_cb, this, std::placeholders::_1));

    // 3. Suscriptor para la posición global (Latitud, Longitud, Altitud)
    gps_sub_ = this->create_subscription<sensor_msgs::msg::NavSatFix>(
      "/mavros/global_position/global", 10, std::bind(&NodoTelemetria::gps_cb, this, std::placeholders::_1));

    // 4. Suscriptor para la velocidad local (m/s)
    vel_sub_ = this->create_subscription<geometry_msgs::msg::TwistStamped>(
      "/mavros/local_position/velocity_local", 10, std::bind(&NodoTelemetria::vel_cb, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "Nodo de Telemetría iniciado. Escuchando a la Pixhawk...");
  }

private:
  // Callbacks: Funciones que se ejecutan automáticamente cada vez que llega un mensaje nuevo

  void state_cb(const mavros_msgs::msg::State::SharedPtr msg)
  {
    // Muestra si la placa está conectada y si los motores están armados
    RCLCPP_INFO(this->get_logger(), "Estado -> Conectado: %s | Armado: %s | Modo: %s",
      msg->connected ? "SI" : "NO",
      msg->armed ? "SI" : "NO",
      msg->mode.c_str());
  }

  void battery_cb(const sensor_msgs::msg::BatteryState::SharedPtr msg)
  {
    // Muestra el voltaje actual y el porcentaje restante
    RCLCPP_INFO(this->get_logger(), "Batería -> Voltaje: %.2f V | Restante: %.0f%%",
      msg->voltage, msg->percentage * 100.0);
  }

  void gps_cb(const sensor_msgs::msg::NavSatFix::SharedPtr msg)
  {
    // Muestra las coordenadas geográficas
    RCLCPP_INFO(this->get_logger(), "GPS -> Lat: %.6f | Lon: %.6f",
      msg->latitude, msg->longitude);
  }

  void vel_cb(const geometry_msgs::msg::TwistStamped::SharedPtr msg)
  {
    // Muestra la velocidad lineal de avance (eje X)
    RCLCPP_INFO(this->get_logger(), "Velocidad -> Avance (X): %.2f m/s",
      msg->twist.linear.x);
  }

  // Declaración de los suscriptores
  rclcpp::Subscription<mavros_msgs::msg::State>::SharedPtr state_sub_;
  rclcpp::Subscription<sensor_msgs::msg::BatteryState>::SharedPtr battery_sub_;
  rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr gps_sub_;
  rclcpp::Subscription<geometry_msgs::msg::TwistStamped>::SharedPtr vel_sub_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<NodoTelemetria>());
  rclcpp::shutdown();
  return 0;
}