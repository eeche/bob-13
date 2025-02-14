package bob.e2e.dto

data class AuthRequest(
    val userInput: String,
    val keyHashMap: Map<String, String>
)