#ifndef __LWIP_THREAD_H__
#define __LWIP_THREAD_H__

//#define RECORD_DEVICE_BOARD
#define TEST_5_BOARD


#ifdef TEST_1_BOARD

/* 用户需要根据设备信息完善以下宏定义中的三元组内容 */
#define PRODUCT_KEY         "i98lEHgwEA1"                                                       /* ProductKey->阿里云颁发的产品唯一标识，11位长度的英文数字随机组合 */
#define DEVICE_NAME         "test_1"                                                       /* DeviceName->用户注册设备时生成的设备唯一编号，支持系统自动生成，也可支持用户添加自定义编号，产品维度内唯一  */
#define DEVICE_SECRET       "a47b6ef51511f71806968848e355a0f6"                                  /* DeviceSecret->设备密钥，与DeviceName成对出现，可用于一机一密的认证方案  */
/* MQTT地址与端口 */
#define HOST_NAME           "iot-06z00ho0nths9re.mqtt.iothub.aliyuncs.com"                  /* 阿里云域名 */
#define HOST_PORT           1883                                                                /* 阿里云域名端口，固定1883 */

#define CLIENT_ID_CONNECT_PARAM "|securemode=2,signmethod=hmacsha256,timestamp=1724225942752|"
/* 根据三元组内容计算得出的数值 */
#define CLIENT_ID           PRODUCT_KEY"."DEVICE_NAME""CLIENT_ID_CONNECT_PARAM  /* 客户端ID */
#define USER_NAME           DEVICE_NAME"&"PRODUCT_KEY                                           /* 客户端用户名 */
#define PASSWORD            "36e97f97e1c6a865af8174b03fe9138e3c287ce61f2ce7d09e88b980d6c21c45"                          /* 由MQTT_Password工具计算得出的连接密码 */
/* 发布与订阅 */
#define DEVICE_PUBLISH      "/"PRODUCT_KEY"/"DEVICE_NAME"/user/update"                          /* 发布 */
#define DEVICE_SUBSCRIBE    "/"PRODUCT_KEY"/"DEVICE_NAME"/user/get"                             /* 订阅 */

#define DEVICE_SENSOR_INFO_POS    "/sys/i98lEHgwEA1/"DEVICE_NAME"/thing/event/property/post"

#endif

#ifdef TEST_2_BOARD

/* 用户需要根据设备信息完善以下宏定义中的三元组内容 */
#define PRODUCT_KEY         "i98lEHgwEA1"                                                       /* ProductKey->阿里云颁发的产品唯一标识，11位长度的英文数字随机组合 */
#define DEVICE_NAME         "test_2"                                                       /* DeviceName->用户注册设备时生成的设备唯一编号，支持系统自动生成，也可支持用户添加自定义编号，产品维度内唯一  */
#define DEVICE_SECRET       "5d358af97c70cd8d5feb08d733df2656"                                  /* DeviceSecret->设备密钥，与DeviceName成对出现，可用于一机一密的认证方案  */
/* MQTT地址与端口 */
#define HOST_NAME           "iot-06z00ho0nths9re.mqtt.iothub.aliyuncs.com"                  /* 阿里云域名 */
#define HOST_PORT           1883                                                                /* 阿里云域名端口，固定1883 */
/* 根据三元组内容计算得出的数值 */
#define CLIENT_ID           "i98lEHgwEA1.test_2|securemode=2,signmethod=hmacsha256,timestamp=1721961835249|"  /* 客户端ID */
#define USER_NAME           DEVICE_NAME"&"PRODUCT_KEY                                           /* 客户端用户名 */
#define PASSWORD            "66aa130cebd5baa4721ae5b5798d370c4582760ce8be953f566b354febe9fcd0"                          /* 由MQTT_Password工具计算得出的连接密码 */
/* 发布与订阅 */
#define DEVICE_PUBLISH      "/"PRODUCT_KEY"/"DEVICE_NAME"/user/update"                          /* 发布 */
#define DEVICE_SUBSCRIBE    "/"PRODUCT_KEY"/"DEVICE_NAME"/user/get"                             /* 订阅 */

#define DEVICE_SENSOR_INFO_POS    "/sys/i98lEHgwEA1/"DEVICE_NAME"/thing/event/property/post"
#endif

#ifdef TEST_3_BOARD

/* 用户需要根据设备信息完善以下宏定义中的三元组内容 */
#define PRODUCT_KEY         "i98lEHgwEA1"                                                       /* ProductKey->阿里云颁发的产品唯一标识，11位长度的英文数字随机组合 */
#define DEVICE_NAME         "test_3"                                                       /* DeviceName->用户注册设备时生成的设备唯一编号，支持系统自动生成，也可支持用户添加自定义编号，产品维度内唯一  */
#define DEVICE_SECRET       "5d358af97c70cd8d5feb08d733df2656"                                  /* DeviceSecret->设备密钥，与DeviceName成对出现，可用于一机一密的认证方案  */
/* MQTT地址与端口 */
#define HOST_NAME           "iot-06z00ho0nths9re.mqtt.iothub.aliyuncs.com"                  /* 阿里云域名 */
#define HOST_PORT           1883                                                                /* 阿里云域名端口，固定1883 */

#define CLIENT_ID_CONNECT_PARAM "|securemode=2,signmethod=hmacsha256,timestamp=1722844066665|"

/* 根据三元组内容计算得出的数值 */
#define CLIENT_ID           PRODUCT_KEY"."DEVICE_NAME""CLIENT_ID_CONNECT_PARAM  /* 客户端ID */
#define USER_NAME           DEVICE_NAME"&"PRODUCT_KEY                                           /* 客户端用户名 */
#define PASSWORD            "cdb6a88a86ddf147cf3794b5219db26a8381af90e7136f04f6803213629ad78d"                          /* 由MQTT_Password工具计算得出的连接密码 */
/* 发布与订阅 */
#define DEVICE_PUBLISH      "/"PRODUCT_KEY"/"DEVICE_NAME"/user/update"                          /* 发布 */
#define DEVICE_SUBSCRIBE    "/"PRODUCT_KEY"/"DEVICE_NAME"/user/get"                             /* 订阅 */

#define DEVICE_SENSOR_INFO_POS    "/sys/i98lEHgwEA1/"DEVICE_NAME"/thing/event/property/post"
#endif

#ifdef TEST_4_BOARD

/* 用户需要根据设备信息完善以下宏定义中的三元组内容 */
#define PRODUCT_KEY         "i98lEHgwEA1"                                                       /* ProductKey->阿里云颁发的产品唯一标识，11位长度的英文数字随机组合 */
#define DEVICE_NAME         "test_4"                                                       /* DeviceName->用户注册设备时生成的设备唯一编号，支持系统自动生成，也可支持用户添加自定义编号，产品维度内唯一  */
#define DEVICE_SECRET       "efca1ce06cb09981bd418f43452bebee"                                  /* DeviceSecret->设备密钥，与DeviceName成对出现，可用于一机一密的认证方案  */
/* MQTT地址与端口 */
#define HOST_NAME           "iot-06z00ho0nths9re.mqtt.iothub.aliyuncs.com"                  /* 阿里云域名 */
#define HOST_PORT           1883                                                                /* 阿里云域名端口，固定1883 */

#define CLIENT_ID_CONNECT_PARAM "|securemode=2,signmethod=hmacsha256,timestamp=1724231923779|"

/* 根据三元组内容计算得出的数值 */
#define CLIENT_ID           PRODUCT_KEY"."DEVICE_NAME""CLIENT_ID_CONNECT_PARAM  /* 客户端ID */
#define USER_NAME           DEVICE_NAME"&"PRODUCT_KEY                                           /* 客户端用户名 */
#define PASSWORD            "bc5655e2713bd227624da04aa2d651a9cb1bd71023d5b06980ae6ad3683175b1"                          /* 由MQTT_Password工具计算得出的连接密码 */
/* 发布与订阅 */
#define DEVICE_PUBLISH      "/"PRODUCT_KEY"/"DEVICE_NAME"/user/update"                          /* 发布 */
#define DEVICE_SUBSCRIBE    "/"PRODUCT_KEY"/"DEVICE_NAME"/user/get"                             /* 订阅 */

#define DEVICE_SENSOR_INFO_POS    "/sys/i98lEHgwEA1/"DEVICE_NAME"/thing/event/property/post"
#endif

#ifdef TEST_5_BOARD

/* 用户需要根据设备信息完善以下宏定义中的三元组内容 */
#define PRODUCT_KEY         "i98lEHgwEA1"                                                       /* ProductKey->阿里云颁发的产品唯一标识，11位长度的英文数字随机组合 */
#define DEVICE_NAME         "test_5"                                                       /* DeviceName->用户注册设备时生成的设备唯一编号，支持系统自动生成，也可支持用户添加自定义编号，产品维度内唯一  */
#define DEVICE_SECRET       "efca1ce06cb09981bd418f43452bebee"                                  /* DeviceSecret->设备密钥，与DeviceName成对出现，可用于一机一密的认证方案  */
/* MQTT地址与端口 */
#define HOST_NAME           "iot-06z00ho0nths9re.mqtt.iothub.aliyuncs.com"                  /* 阿里云域名 */
#define HOST_PORT           1883                                                                /* 阿里云域名端口，固定1883 */

#define CLIENT_ID_CONNECT_PARAM "|securemode=2,signmethod=hmacsha256,timestamp=1726108013061|"

/* 根据三元组内容计算得出的数值 */
#define CLIENT_ID           PRODUCT_KEY"."DEVICE_NAME""CLIENT_ID_CONNECT_PARAM  /* 客户端ID */
#define USER_NAME           DEVICE_NAME"&"PRODUCT_KEY                                           /* 客户端用户名 */
#define PASSWORD            "a453cf77181e9f3bee10a0e7423969fe39dc7438684491389e9bcd4aaf55e769"                          /* 由MQTT_Password工具计算得出的连接密码 */
/* 发布与订阅 */
#define DEVICE_PUBLISH      "/"PRODUCT_KEY"/"DEVICE_NAME"/user/update"                          /* 发布 */
#define DEVICE_SUBSCRIBE    "/"PRODUCT_KEY"/"DEVICE_NAME"/user/get"                             /* 订阅 */

#define DEVICE_SENSOR_INFO_POS    "/sys/i98lEHgwEA1/"DEVICE_NAME"/thing/event/property/post"
#endif


#ifdef TEST_JUST_BOARD

/* 用户需要根据设备信息完善以下宏定义中的三元组内容 */
#define PRODUCT_KEY         "i98lEHgwEA1"                                                       /* ProductKey->阿里云颁发的产品唯一标识，11位长度的英文数字随机组合 */
#define DEVICE_NAME         "just_test_for_other"                                                       /* DeviceName->用户注册设备时生成的设备唯一编号，支持系统自动生成，也可支持用户添加自定义编号，产品维度内唯一  */
#define DEVICE_SECRET       "6cc67782b333b98cd7dc272e8d0c353a"                                  /* DeviceSecret->设备密钥，与DeviceName成对出现，可用于一机一密的认证方案  */
/* MQTT地址与端口 */
#define HOST_NAME           "iot-06z00ho0nths9re.mqtt.iothub.aliyuncs.com"                  /* 阿里云域名 */
#define HOST_PORT           1883                                                                /* 阿里云域名端口，固定1883 */

#define CLIENT_ID_CONNECT_PARAM "|securemode=2,signmethod=hmacsha256,timestamp=1724296816097|"

/* 根据三元组内容计算得出的数值 */
#define CLIENT_ID           PRODUCT_KEY"."DEVICE_NAME""CLIENT_ID_CONNECT_PARAM  /* 客户端ID */
#define USER_NAME           DEVICE_NAME"&"PRODUCT_KEY                                           /* 客户端用户名 */
#define PASSWORD            "820627d1a2d32a70fd3ee40c75f7b63a78b8a7b0da250b37617c192d08be4bcd"                          /* 由MQTT_Password工具计算得出的连接密码 */
/* 发布与订阅 */
#define DEVICE_PUBLISH      "/"PRODUCT_KEY"/"DEVICE_NAME"/user/update"                          /* 发布 */
#define DEVICE_SUBSCRIBE    "/"PRODUCT_KEY"/"DEVICE_NAME"/user/get"                             /* 订阅 */

#define DEVICE_SENSOR_INFO_POS    "/sys/i98lEHgwEA1/"DEVICE_NAME"/thing/event/property/post"
#endif

#ifdef RECORD_DEVICE_BOARD

/* 用户需要根据设备信息完善以下宏定义中的三元组内容 */
#define PRODUCT_KEY         "i98lEHgwEA1"                                                       /* ProductKey->阿里云颁发的产品唯一标识，11位长度的英文数字随机组合 */
#define DEVICE_NAME         "record_device"                                                       /* DeviceName->用户注册设备时生成的设备唯一编号，支持系统自动生成，也可支持用户添加自定义编号，产品维度内唯一  */
#define DEVICE_SECRET       "e87b0a3087ee6b5bffbc5868cb91e202"                                  /* DeviceSecret->设备密钥，与DeviceName成对出现，可用于一机一密的认证方案  */
/* MQTT地址与端口 */
#define HOST_NAME           "iot-06z00ho0nths9re.mqtt.iothub.aliyuncs.com"                  /* 阿里云域名 */
#define HOST_PORT           1883                                                                /* 阿里云域名端口，固定1883 */

#define CLIENT_ID_CONNECT_PARAM "|securemode=2,signmethod=hmacsha256,timestamp=1724393552805|"

/* 根据三元组内容计算得出的数值 */
#define CLIENT_ID           PRODUCT_KEY"."DEVICE_NAME""CLIENT_ID_CONNECT_PARAM  /* 客户端ID */
#define USER_NAME           DEVICE_NAME"&"PRODUCT_KEY                                           /* 客户端用户名 */
#define PASSWORD            "4383368a96c3887e794df0ef7edbaccbf863282d8ca5b0b85d13c7e36023272d"                          /* 由MQTT_Password工具计算得出的连接密码 */
/* 发布与订阅 */
#define DEVICE_PUBLISH      "/"PRODUCT_KEY"/"DEVICE_NAME"/user/update"                          /* 发布 */
#define DEVICE_SUBSCRIBE    "/"PRODUCT_KEY"/"DEVICE_NAME"/user/get"                             /* 订阅 */

#define DEVICE_SENSOR_INFO_POS    "/sys/i98lEHgwEA1/"DEVICE_NAME"/thing/event/property/post"
#endif


#ifdef INPUT_DEVICE_BOARD

/* 用户需要根据设备信息完善以下宏定义中的三元组内容 */
#define PRODUCT_KEY         "i98lEHgwEA1"                                                       /* ProductKey->阿里云颁发的产品唯一标识，11位长度的英文数字随机组合 */
#define DEVICE_NAME         "input_device"                                                       /* DeviceName->用户注册设备时生成的设备唯一编号，支持系统自动生成，也可支持用户添加自定义编号，产品维度内唯一  */
#define DEVICE_SECRET       "34c5fcd0301601157654d83cda51cee6"                                  /* DeviceSecret->设备密钥，与DeviceName成对出现，可用于一机一密的认证方案  */
/* MQTT地址与端口 */
#define HOST_NAME           "iot-06z00ho0nths9re.mqtt.iothub.aliyuncs.com"                  /* 阿里云域名 */
#define HOST_PORT           1883                                                                /* 阿里云域名端口，固定1883 */

#define CLIENT_ID_CONNECT_PARAM "|securemode=2,signmethod=hmacsha256,timestamp=1725955487709|"

/* 根据三元组内容计算得出的数值 */
#define CLIENT_ID           PRODUCT_KEY"."DEVICE_NAME""CLIENT_ID_CONNECT_PARAM  /* 客户端ID */
#define USER_NAME           DEVICE_NAME"&"PRODUCT_KEY                                           /* 客户端用户名 */
#define PASSWORD            "67bc31f71c15ce84d0832f08eafb3ac7ba735fea9d585956e24b90bc5e475363"                          /* 由MQTT_Password工具计算得出的连接密码 */
/* 发布与订阅 */
#define DEVICE_PUBLISH      "/"PRODUCT_KEY"/"DEVICE_NAME"/user/update"                          /* 发布 */
#define DEVICE_SUBSCRIBE    "/"PRODUCT_KEY"/"DEVICE_NAME"/user/get"                             /* 订阅 */

#define DEVICE_SENSOR_INFO_POS    "/sys/i98lEHgwEA1/"DEVICE_NAME"/thing/event/property/post"
#endif

void lwip_thread(void *pvparams);

#endif
