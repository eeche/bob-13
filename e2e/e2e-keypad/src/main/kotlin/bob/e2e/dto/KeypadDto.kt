package bob.e2e.dto

data class KeypadResponse(
    val keypadImage: String,
    val hashList: List<String>,
    val sessionId: String
)