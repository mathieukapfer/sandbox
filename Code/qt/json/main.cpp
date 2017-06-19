#include <iostream>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>

#include <QtCore/QUuid>

#define TEST_THIS(cmd) std::cout << #cmd << ":" << (cmd) << std::endl;


void Decode() {
  QByteArray msgRaw = "[2,\"090c417b-1943-4c22-b1d5-172a15119eb5\",\"BootNotification\",{\"chargeBoxSerialNumber\":\"123456\",\"chargePointModel\":\"AdvancedWB\",\"chargePointSerialNumber\":\"\",\"chargePointVendor\":\"Schneider\",\"firmwareVersion\":\"\",\"iccid\":\"\",\"imsi\":\"\",\"meterSerialNumber\":\"\",\"meterType\":\"\"}]";

  // read and write for the fun
  QJsonDocument msgJson(QJsonDocument::fromJson(msgRaw));
  QJsonArray msgArray = msgJson.array();

  TEST_THIS(msgJson.toJson().constData());

  TEST_THIS(msgJson.isArray());
  TEST_THIS(msgJson.array().size());
  TEST_THIS(msgArray.size());
  TEST_THIS(msgJson.isObject());

  TEST_THIS(msgArray[0].isDouble());
  TEST_THIS(msgArray[0].toDouble());
  TEST_THIS(msgArray[1].isString());
  TEST_THIS(msgArray[1].toString().toStdString());
  TEST_THIS(msgArray[2].toString().toStdString());
  TEST_THIS(msgArray[3].isObject());

  QJsonObject msgBody = msgArray[3].toObject();
  
//TEST_THIS(msgBody.find("chargeBoxSerialNumberXXX") == QJsonObject::const_iterator::end());
  TEST_THIS(*msgBody.find("chargeBoxSerialNumberXXX") == QJsonValue::Null);
  TEST_THIS(msgBody["chargeBoxSerialNumberXXX"] == QJsonValue::Null);
  TEST_THIS(msgBody["chargeBoxSerialNumberXXX"].toString().toStdString());
  TEST_THIS(msgBody["chargeBoxSerialNumberXXX"].toString().size());
  TEST_THIS(msgBody["chargeBoxSerialNumber"].toString().size());
  TEST_THIS(msgBody["chargeBoxSerialNumber"].toString().toStdString());
}

void Encode_(QJsonObject &obj, const std::string& p_key, const std::string& p_value) {
  obj[QString::fromStdString(p_key)] = QString::fromStdString(p_value);
}

void Encode_(QJsonObject &obj, const std::string& p_key, const int& p_value) {
  obj[QString::fromStdString(p_key)] = p_value;
}

void EncodeObject() {
  QJsonObject msgBody;
  QJsonDocument doc;
  msgBody["chargeBoxSerialNumber"] = QString("123456");
  Encode_(msgBody, "chargePointModel", "AdvancedWB");
  Encode_(msgBody, "thisIsANum", 123);

  doc.setObject(msgBody);

  TEST_THIS(doc.toJson().constData());

  QString msgStr(doc.toJson());
  for (int c=0; c < msgStr.size(); c++) { printf("%c", msgStr.toStdString().c_str()[c]); };

}

void EncodeArray() {
  QJsonDocument doc;
  QJsonArray msgArray;
  QJsonObject val1{ {"prop1", 1 } };
  QJsonObject val2{ {"prop2", 2 } };

  msgArray.append(val1);
  msgArray.append(val2);

  doc.setArray(msgArray);

  TEST_THIS(doc.toJson().constData());
}

static const int MESSAGE_TYPE_CALL = 1;

bool send() {

  const std::string p_action = "action";
  QJsonObject p_payloadCall{ {"payloadCall", 1 } };
  QJsonArray message;
  //=====
  QString uniqueId = QUuid::createUuid().toString();
  QString action = QString::fromStdString(p_action);

  message.append(QJsonValue(MESSAGE_TYPE_CALL));
  message.append(QJsonValue(uniqueId)); // UniqueId
  message.append(QJsonValue(action));   // Action
  message.append(p_payloadCall);        // payload

  QJsonDocument doc;
  doc.setArray(message);
  QString messageToString(doc.toJson());

  //=====
  TEST_THIS(messageToString.toStdString());

}

int main(int argc, char *argv[]) {
  QJsonArray msgArray;
  TEST_THIS(msgArray.size());
  TEST_THIS(msgArray.isEmpty());

  //Decode();
  EncodeObject();
  //EncodeArray();
  //send();
  return 0;
}
