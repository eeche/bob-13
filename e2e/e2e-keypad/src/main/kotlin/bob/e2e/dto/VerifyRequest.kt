package bob.e2e.dto

data class VerifyRequest(
    val encryptedData: String,
    val sessionId: String
)