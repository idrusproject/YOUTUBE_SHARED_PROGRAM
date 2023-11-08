void handleNewMessages(int numNewMessages)
{
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") {
      from_name = "Guest";
    }

    if (text == "/start") {
      String welcome = "Hi, " + from_name + ".\n";
      welcome += "Akses kontrol :\n";
      welcome += "/" + idDevice + "_on " + "Untuk menyalakan lampu.\n";
      welcome += "/" + idDevice + "_off " + "Untuk mematikan lampu.\n";

      bot.sendMessage(chat_id, welcome);
    }

    if (text.equals("/" + idDevice + "_on")) { // Corrected the closing parenthesis
      digitalWrite(relay, 0);
      String welcome = "Lampu Menyala.\n";
      bot.sendMessage(chat_id, welcome);
    } else if (text.equals("/" + idDevice + "_off")) {
      digitalWrite(relay, 1);
      String welcome = "Lampu Mati.\n"; // Corrected the message for "off" case
      bot.sendMessage(chat_id, welcome);
    }
  }
}
