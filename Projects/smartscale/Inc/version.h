/**
  ******************************************************************************
  * @file    version.h
  * @author  MMY Application Team
  * @brief   Header for firmware version number
  ******************************************************************************
  * @attention
  *
  * COPYRIGHT 2021 STMicroelectronics, all rights reserved
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
  * AND SPECIFICALLY DISCLAIMING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _VERSION_H
#define _VERSION_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define PRODUCT_DEVICE_NAME               "smartscale"

#define TOSTRING(s) #s
#define STRINGIZE(s) TOSTRING(s)

#define MCU_FW_VERSION_MAX                1
#define MCU_FW_VERSION_MAJOR              1
#define MCU_FW_VERSION_MINOR              0
#define MCU_FW_VERSION_REVISION           0
#define MCU_FW_VERSION                    STRINGIZE(MCU_FW_VERSION_MAX) "." \
                                          STRINGIZE(MCU_FW_VERSION_MAJOR) "." \
                                          STRINGIZE(MCU_FW_VERSION_MINOR) "." \
                                          STRINGIZE(MCU_FW_VERSION_REVISION)

#define MCU_HW_VERSION                    "1.0.0.0"


extern const char CodeBuildDate[];
extern const char CodeBuildTime[];

/* Exported functions ------------------------------------------------------- */

#endif /* _VERSION_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
