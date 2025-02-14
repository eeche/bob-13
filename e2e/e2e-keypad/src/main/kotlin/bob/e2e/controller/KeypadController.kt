package bob.e2e.controller

import bob.e2e.dto.KeypadResponse
import bob.e2e.dto.VerifyRequest
import bob.e2e.service.KeypadService
import org.springframework.http.ResponseEntity
import org.springframework.web.bind.annotation.*

@RequestMapping("/keypad")
@RestController
class KeypadController(private val keypadService: KeypadService) {

    @GetMapping
    fun getKeypad(): ResponseEntity<KeypadResponse> {
        val (base64Image, hashList, sessionId) = keypadService.generateKeypadImageAndHash()
        return ResponseEntity.ok(KeypadResponse(base64Image, hashList, sessionId))
    }

    @PostMapping("/verify")
    fun verifyInput(@RequestBody request: VerifyRequest): String {
        val authResponse = keypadService.sendAuthRequest(request.encryptedData, request.sessionId)
        return authResponse
    }

    @GetMapping("/test-redis")
    fun testRedis(): ResponseEntity<String> {
        keypadService.testRedisConnection()
        return ResponseEntity.ok("Redis connection test completed. Check console for results.")
    }
}