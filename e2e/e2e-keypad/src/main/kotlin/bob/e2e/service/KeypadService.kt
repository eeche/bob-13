package bob.e2e.service

import bob.e2e.dto.AuthRequest
import bob.e2e.repository.KeypadRedisRepository
import org.springframework.beans.factory.annotation.Autowired
import org.springframework.core.io.ClassPathResource
import org.springframework.data.redis.core.RedisTemplate
import org.springframework.stereotype.Service
import org.springframework.web.client.RestTemplate
import java.awt.Color
import java.awt.image.BufferedImage
import java.io.ByteArrayOutputStream
import java.security.MessageDigest
import java.util.*
import javax.imageio.ImageIO

@Service
class KeypadService(private val keypadRedisRepository: KeypadRedisRepository,
                    private val restTemplate: RestTemplate
) {
    private val numbers = (0..9).toList()
    private val buttonPositions = listOf(
        Pair(0, 0), Pair(50, 0), Pair(100, 0), Pair(150, 0),
        Pair(0, 50), Pair(50, 50), Pair(100, 50), Pair(150, 50),
        Pair(0, 100), Pair(50, 100), Pair(100, 100), Pair(150, 100)
    )

    fun generateKeypadImageAndHash(): Triple<String, List<String>, String> {
        val shuffledButtons = (numbers + listOf("blank", "blank")).shuffled()
        val keypadImage = createKeypadImage(shuffledButtons)
        val base64Image = convertToBase64(keypadImage)
        val hashList = createHashList(shuffledButtons)
        val sessionId = UUID.randomUUID().toString()

        keypadRedisRepository.saveKeypadData(sessionId, shuffledButtons.map { it.toString() }, hashList)

        return Triple(base64Image, hashList, sessionId)
    }

    private fun createKeypadImage(shuffledButtons: List<Any>): BufferedImage {
        val buttonWidth = 50
        val buttonHeight = 50
        val keypadWidth = buttonWidth * 4
        val keypadHeight = buttonHeight * 3

        val keypadImage = BufferedImage(keypadWidth, keypadHeight, BufferedImage.TYPE_INT_ARGB)
        val g2d = keypadImage.createGraphics()
        g2d.color = Color.WHITE
        g2d.fillRect(0, 0, keypadWidth, keypadHeight)

        shuffledButtons.forEachIndexed { index, button ->
            val buttonImage = when (button) {
                "blank" -> loadImage("keypad/_blank.png")
                else -> loadImage("keypad/_$button.png")
            }
            val (x, y) = buttonPositions[index]
            g2d.drawImage(buttonImage, x, y, null)
        }
        g2d.dispose()

        return keypadImage
    }

    private fun loadImage(path: String): BufferedImage {
        val resource = ClassPathResource(path)
        return ImageIO.read(resource.inputStream) ?: throw IllegalStateException("Failed to load image: $path")
    }

    private fun convertToBase64(image: BufferedImage): String {
        val outputStream = ByteArrayOutputStream()
        if (!ImageIO.write(image, "png", outputStream)) {
            throw IllegalStateException("Failed to write image as PNG")
        }
        return Base64.getEncoder().encodeToString(outputStream.toByteArray())
    }

//    shuffled 된 버튼 리스트를 받아 각 버튼에 대한 해시값 리스트를 생성
    private fun createHashList(shuffledButtons: List<Any>): List<String> {
        return shuffledButtons.map { button ->
            when (button) {
                "blank" -> ""
                else -> hashButton(button.toString())
            }
        }
    }

//    SHA-256 알고리즘으로 해시화
    private fun hashButton(buttonValue: String): String {
        val md = MessageDigest.getInstance("SHA-1")
        val hashBytes = md.digest(buttonValue.toByteArray())
        return hashBytes.joinToString("") { "%02x".format(it) }
    }
    @Autowired
    private lateinit var redisTemplate: RedisTemplate<String, String>

    fun sendAuthRequest(encryptedData: String, sessionId: String): String {
        val keyHashMap = keypadRedisRepository.getKeyHashMap(sessionId)
        val authRequest = AuthRequest(
            userInput = encryptedData,
            keyHashMap = keyHashMap
        )

        val authUrl = "http://146.56.119.112:8081/auth"
        val response = restTemplate.postForObject(authUrl, authRequest, String::class.java)
        return response ?: throw RuntimeException("Failed to get response from auth endpoint")
    }

    fun testRedisConnection() {
        try {
            redisTemplate.opsForValue().set("test", "Hello Redis")
            val value = redisTemplate.opsForValue().get("test")
            println("Retrieved value from Redis: $value")
        } catch (e: Exception) {
            println("Failed to connect to Redis: ${e.message}")
        }
    }
}